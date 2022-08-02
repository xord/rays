# -*- coding: utf-8 -*-


require_relative 'helper'


class TestImage < Test::Unit::TestCase

  def image(*args)
    Rays::Image.new(*args)
  end

  def load(path)
    Rays::Image.load path
  end

  def color(r = 0, g = 0, b = 0, a = 0)
    Rays::Color.new r, g, b, a
  end

  def bounds(*args)
    Rays::Bounds.new(*args)
  end

  def test_initialize()
    assert_equal 10, image(10, 20).width
    assert_equal 10, image(20, 10).height
  end

  def test_dup()
    o          = image 10, 10
    assert_equal color(0, 0, 0, 0), o[0, 0]
    o[0, 0]    = color(1, 0, 0, 0)
    assert_equal color(1, 0, 0, 0), o[0, 0]
    x          = o.dup
    assert_equal color(1, 0, 0, 0), x[0, 0]
    x[0, 0]    = color(0, 1, 0, 0)
    assert_equal color(0, 1, 0, 0), x[0, 0]
    assert_equal color(1, 0, 0, 0), o[0, 0]
  end

  def test_bitmap()
    assert_equal 10, image(10, 20).bitmap.width
    assert_equal 10, image(20, 10).bitmap.height
  end

  def test_painter()
    pa = image(10, 10).painter
    assert_equal color(0, 0, 0, 0), pa.background
    assert_equal color(1, 1, 1, 1), pa.fill
    assert_equal color(1, 1, 1, 0), pa.stroke
    assert_equal bounds(0, 0, -1, -1), pa.clip
    assert_equal Rays::Font.new, pa.font
  end

  def test_paint()
    def paint(&block)
      Rays::Image.new(10, 10).paint(&block)
    end
    def fill(&block)
      paint {|p| p.fill 1, 0, 0; p.stroke nil; block.call p}
    end
    def stroke(&block)
      paint {|p| p.fill nil; p.stroke 1, 0, 0; block.call p}
    end
    def drawn(&block)
      fill(&block).bitmap.to_a.reject {|o| o.transparent?}.uniq.size > 0
    end

    assert_equal color(0, 0, 0, 0), fill   {|p| p.rect 1, 1, 8, 8}[0, 0]
    assert_equal color(1, 0, 0, 1), fill   {|p| p.rect 1, 1, 8, 8}[1, 1]
    assert_equal color(1, 0, 0, 1), stroke {|p| p.line 0, 0, 1, 1}[0, 0]

    assert drawn {|p| p.text "a"}
  end

end# TestImage
