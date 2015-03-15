# -*- coding: utf-8 -*-


require_relative 'helper'


class TestTexture < Test::Unit::TestCase

  W = 32
  H = 16

  def tex (w = W, h = H)
    Rays::Texture.new w, h
  end

  def test_initialize ()
    assert_equal W, tex.width
    assert_equal H, tex.height
  end

  def test_to_bitmap ()
    assert_equal Rays::Bitmap, tex.to_bitmap.class
    assert_equal W, tex.to_bitmap.width
    assert_equal H, tex.to_bitmap.height
  end

end# TestTexture
