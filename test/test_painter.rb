# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPainter < Test::Unit::TestCase

  def painter ()
    Rays::Painter.new
  end

  def font (name = nil, size = nil)
    Rays::Font.new name, size
  end

  def rgb (*args)
    Rays::Color.new *args
  end

  def test_background_accessor ()
    pa = painter
    pa.background = 1
    assert_equal rgb(1, 1, 1, 1), pa.background
    pa.background = 0
    assert_equal rgb(0, 0, 0, 1), pa.background
    pa.background 1
    assert_equal rgb(1, 1, 1, 1), pa.background
    assert_equal rgb(1, 1, 1, 1), pa.background(0) {|_|
      assert_equal rgb(0, 0, 0, 1), pa.background
    }
    assert_equal rgb(1, 1, 1, 1), pa.background
  end

  def test_fill_accessor ()
    pa = painter
    pa.fill = 1
    assert_equal rgb(1, 1, 1, 1), pa.fill
    pa.fill = 0
    assert_equal rgb(0, 0, 0, 1), pa.fill
    pa.fill 1
    assert_equal rgb(1, 1, 1, 1), pa.fill
    assert_equal rgb(1, 1, 1, 1), pa.fill(0) {|_|
      assert_equal rgb(0, 0, 0, 1), pa.fill
    }
    assert_equal rgb(1, 1, 1, 1), pa.fill
  end

  def test_stroke_accessor ()
    pa = painter
    pa.stroke = 1
    assert_equal rgb(1, 1, 1, 1), pa.stroke
    pa.stroke = 0
    assert_equal rgb(0, 0, 0, 1), pa.stroke
    pa.stroke 1
    assert_equal rgb(1, 1, 1, 1), pa.stroke
    assert_equal rgb(1, 1, 1, 1), pa.stroke(0) {|_|
      assert_equal rgb(0, 0, 0, 1), pa.stroke
    }
    assert_equal rgb(1, 1, 1, 1), pa.stroke
  end

  def test_clip_accessor ()
    pa = painter
    pa.clip = [1, 2, 3, 4]
    assert_equal [1, 2, 3, 4], pa.clip.to_a
    pa.clip = [5, 6, 7, 8]
    assert_equal [5, 6, 7, 8], pa.clip.to_a
    pa.clip 1, 2, 3, 4
    assert_equal [1, 2, 3, 4], pa.clip.to_a
    assert_equal [1, 2, 3, 4], pa.clip(5, 6, 7, 8) {|_|
      assert_equal [5, 6, 7, 8], pa.clip.to_a
    }.to_a
    assert_equal [1, 2, 3, 4], pa.clip.to_a
  end

  def test_font_accessor ()
    pa = painter
    f10, f20 = font(nil, 10), font(nil, 20)
    pa.font = f10
    assert_equal f10, pa.font
    pa.font = f20
    assert_equal f20, pa.font
    pa.font f10
    assert_equal f10, pa.font
    assert_equal f10, pa.font(f20) {|_|
      assert_equal f20, pa.font
    }
    assert_equal f10, pa.font
  end

  def test_font_name_size ()
    pa = painter
    pa.font "Menlo", 10
    assert_equal "Menlo Regular", pa.font.name
    assert_equal 10, pa.font.size
    pa.font nil
    assert_not_equal "Menlo Regular", pa.font.name
    pa.font nil, 20
    assert_equal 20, pa.font.size
  end

  def test_color_accessor ()
    pa = painter
    f1, f2, s1, s2 = rgb(1, 0, 0), rgb(0, 1, 0), rgb(1, 0, 1), rgb(0, 1, 1)
    pa.color f1, s1
    assert_equal f1, pa.fill
    assert_equal s1, pa.stroke
    assert_equal [f1, s1], pa.color(f2, s2) {|_|
      assert_equal f2, pa.fill
      assert_equal s2, pa.stroke
    }
    assert_equal f1, pa.fill
    assert_equal s1, pa.stroke
  end

  def test_color_by_name ()
    pa = painter
    pa.fill = [1, 0, 0]
    assert_equal rgb(1, 0, 0), pa.fill
    pa.fill = :red
    assert_equal rgb(1, 0, 0), pa.fill
    pa.fill = '#f00'
    assert_equal rgb(1, 0, 0), pa.fill
    pa.fill = '#ff0000'
    assert_equal rgb(1, 0, 0), pa.fill
  end

end# TestPainter
