# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPolylineLine < Test::Unit::TestCase

  def rect (*args)
    Rays::Polygon.rect *args
  end
            
  def test_initialize ()
    assert_raise(NotImplementedError) {Rays::Polygon::Line.new}
  end

  def test_hole ()
    o = rect(0, 0, 10, 10) - rect(1, 1, 2, 2)
    assert_equal 1, o.select {|line| line.hole?}.size

    o = rect(0, 0, 10, 10) - rect(100, 1, 2, 2)
    assert_equal 0, o.select {|line| line.hole?}.size
  end

end# TestPolyline
