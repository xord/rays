# -*- coding: utf-8 -*-


require_relative 'helper'


class TestBitmap < Test::Unit::TestCase

  W = 32
  H = 16

  def bmp (w = W, h = H)
    Rays::Bitmap.new w, h
  end

  def test_initialize ()
    assert_equal W, bmp.width
    assert_equal H, bmp.height
  end

  def test_to_texture ()
    assert_equal Rays::Texture, bmp.to_texture.class
    assert_equal W, bmp.to_texture.width
    assert_equal H, bmp.to_texture.height
  end

end# TestBitmap
