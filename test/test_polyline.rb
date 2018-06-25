# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPolyline < Test::Unit::TestCase

  class Polyline < Rays::Polyline
    def initialize (*args, loop: false)
      set_points args, loop
    end

    def dump ()
      map {|point| point.to_a}
    end
  end

  def polyline (*args)
    Polyline.new *args
  end

  def point (*args)
    Rays::Point.new *args
  end

  def test_initialize ()
    assert_equal [[1, 2], [3, 4]], polyline(   1, 2,     3, 4 ).dump
    assert_equal [[5, 6], [7, 8]], polyline(  [5, 6],   [7, 8]).dump
    assert_equal [[1, 1], [2, 2]], polyline(     [1],      [2]).dump
    assert_equal [[3, 3], [4, 4]], polyline(point(3), point(4)).dump
    assert_nothing_raised       {polyline(                  loop: true)}
    assert_nothing_raised       {polyline(                  loop: false)}
    assert_raise(ArgumentError) {polyline(1,                loop: true)}
    assert_raise(ArgumentError) {polyline(1,                loop: false)}
    assert_raise(ArgumentError) {polyline(1, 2,             loop: true)}
    assert_raise(ArgumentError) {polyline(1, 2,             loop: false)}
    assert_raise(ArgumentError) {polyline(1, 2, 3,          loop: true)}
    assert_raise(ArgumentError) {polyline(1, 2, 3,          loop: false)}
    assert_raise(ArgumentError) {polyline(1, 2, 3, 4,       loop: true)}
    assert_nothing_raised       {polyline(1, 2, 3, 4,       loop: false)}
    assert_raise(ArgumentError) {polyline(1, 2, 3, 4, 5,    loop: true)}
    assert_raise(ArgumentError) {polyline(1, 2, 3, 4, 5,    loop: false)}
    assert_nothing_raised       {polyline(1, 2, 3, 4, 5, 6, loop: true)}
    assert_nothing_raised       {polyline(1, 2, 3, 4, 5, 6, loop: false)}
  end

  def test_loop ()
    assert_equal true,  polyline(1, 2, 3, 4, 5, 6, loop: true ).loop?
    assert_equal false, polyline(1, 2, 3, 4, 5, 6, loop: false).loop?
    assert_equal false, polyline(                  loop: true ).loop?
    assert_equal false, polyline(                  loop: false).loop?
  end

  def test_size ()
    assert_equal 2, polyline(1, 2, 3, 4,     ).size
    assert_equal 3, polyline(1, 2, 3, 4, 5, 6).size
  end

  def test_empty? ()
    assert_equal true,  polyline(          ).empty?
    assert_equal false, polyline(1, 2, 3, 4).empty?
  end

  def test_index ()
    o = polyline 1, 2, 3, 4, 5, 6
    assert_equal [1, 2], o[ 0].to_a
    assert_equal [3, 4], o[ 1].to_a
    assert_equal [5, 6], o[-1].to_a
    assert_raise(IndexError) {o[ 3]}
    assert_raise(IndexError) {o[-4]}
  end

  def test_inspect ()
    assert_equal(
      "#<Rays::Polyline [1.0, 2.0], [3.0, 4.0], loop: false>",
      polyline(1, 2, 3, 4).inspect)
  end

end# TestPolyline
