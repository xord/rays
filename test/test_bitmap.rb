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

end# TestBitmap
