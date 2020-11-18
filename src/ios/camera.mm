// -*- mode: objc -*-
#import "rays/camera.h"


#import <AVFoundation/AVFoundation.h>
#include "bitmap.h"


static int video_input_queue_index = 0;


@interface VideoInput : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
@end


@implementation VideoInput

	{
		AVCaptureSession* captureSession;
		dispatch_queue_t captureQueue;
		CGImageRef captureImage;
	}

	- (id) init
	{
		self = [super init];
		if (self)
		{
			captureSession = nil;
			captureQueue   = nil;
			captureImage   = nil;
		}
		return self;
	}

	- (void) dealloc
	{
		[self stop];
		[self clearImage];

		if (captureQueue)
		{
			dispatch_release(captureQueue);
			captureQueue = nil;
		}

		[super dealloc];
	}

	- (dispatch_queue_t) queue
	{
		if (!captureQueue)
		{
			auto name = Xot::stringf(
				"org.xord.RaysVideoInputQueue_%d",
				video_input_queue_index++);
			captureQueue = dispatch_queue_create(name, DISPATCH_QUEUE_SERIAL);
		}
		return captureQueue;
	}

	- (BOOL) start
	{
		[self stop];

		AVCaptureSession* session = [[[AVCaptureSession alloc] init] autorelease];
		session.sessionPreset = AVCaptureSessionPresetHigh;

		AVCaptureDevice* device =
			[AVCaptureDevice defaultDeviceWithMediaType: AVMediaTypeVideo];
		if (!device) return NO;

		//device.activeVideoMinFrameDuration = CMTimeMake(1, 30);

		NSError* error = nil;
		AVCaptureDeviceInput* input = [[[AVCaptureDeviceInput alloc]
			initWithDevice: device error: &error]
			autorelease];
		if (!input || error || ![session canAddInput: input])
			return NO;

		AVCaptureVideoDataOutput* output =
			[[[AVCaptureVideoDataOutput alloc] init] autorelease];
		output.videoSettings = @{
			(NSString*) kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)
		};
		output.alwaysDiscardsLateVideoFrames = YES;
		[output setSampleBufferDelegate: self queue: self.queue];
		if (![session canAddOutput: output])
			return NO;

		[session addInput: input];
		[session addOutput: output];
		[session startRunning];

		captureSession = [session retain];
		return YES;
	}

	- (void) captureOutput: (AVCaptureOutput*) output
		didOutputSampleBuffer: (CMSampleBufferRef) sampleBuffer
		fromConnection: (AVCaptureConnection*) connection
	{
		CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
		if (!pixelBuffer) return;

		CIImage* ciImage = [CIImage imageWithCVPixelBuffer: pixelBuffer];
		if (!ciImage) return;

		CIContext* context = [CIContext contextWithOptions: nil];
		size_t width       = CVPixelBufferGetWidth(pixelBuffer);
		size_t height      = CVPixelBufferGetHeight(pixelBuffer);
		CGRect rect        = CGRectMake(0, 0, width, height);
		CGImageRef cgImage = [context createCGImage: ciImage fromRect: rect];

		dispatch_async(dispatch_get_main_queue(), ^{
			[self clearImage];
			captureImage = cgImage;
		});
	}

	- (void) stop
	{
		if (!captureSession) return;

		[captureSession stopRunning];
		[captureSession release];
		captureSession = nil;
	}

	- (BOOL) isActive
	{
		return captureSession != nil;
	}

	- (void) clearImage
	{
		if (!captureImage) return;

		CGImageRelease(captureImage);
		captureImage = nil;
	}

	- (CGImageRef) getImage
	{
		return captureImage;
	}

@end// VideoInput


namespace Rays
{


	struct Camera::Data
	{

		mutable Image image;

		VideoInput* video_input = nil;

		void update_image_from_video_input () const
		{
			if (!video_input) return;

			CGImageRef cgImage = [video_input getImage];
			if (!cgImage) return;

			if (!image)
			{
				Bitmap bmp(
					(int) CGImageGetWidth(cgImage),
					(int) CGImageGetHeight(cgImage));
				image = Image(bmp);
			}

			Bitmap_copy_pixels(&image.bitmap(), cgImage);

			[video_input clearImage];
		}

	};// Camera::Data


	Camera::Camera ()
	{
	}

	Camera::~Camera ()
	{
		stop();
		if (self->video_input) [self->video_input release];
	}

	bool
	Camera::start ()
	{
		if (!self->video_input) self->video_input = [[VideoInput alloc] init];
		return [self->video_input start];
	}

	void
	Camera::stop ()
	{
		if (!self->video_input) return;

		[self->video_input stop];
	}

	bool
	Camera::is_active () const
	{
		return self->video_input && [self->video_input isActive];
	}

	const Image*
	Camera::image () const
	{
		self->update_image_from_video_input();
		return self->image ? &self->image : NULL;
	}

	Camera::operator bool () const
	{
		return true;
	}

	bool
	Camera::operator ! () const
	{
		return !operator bool();
	}


}// Rays
