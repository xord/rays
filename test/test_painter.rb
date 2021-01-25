# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPainter < Test::Unit::TestCase

  def painter()
    Rays::Painter.new
  end

  def font(name = nil, size = nil)
    Rays::Font.new name, size
  end

  def color(*args)
    Rays::Color.new(*args)
  end

  def setup()
    Rays::Color.set_palette_color :rgb001, color(0, 0, 1)
  end

  def test_background_accessor()
    pa = painter
    pa.background = 1
    assert_equal   color(1, 1, 1, 1), pa.background
    pa.background = 0
    assert_equal   color(0, 0, 0, 1), pa.background
    pa.background   1
    assert_equal   color(1, 1, 1, 1), pa.background
    pa.push background: 0 do |_|
      assert_equal color(0, 0, 0, 1), pa.background
    end
    assert_equal   color(1, 1, 1, 1), pa.background
  end

  def test_fill_accessor()
    pa = painter
    pa.fill = 1
    assert_equal   color(1, 1, 1, 1), pa.fill
    pa.fill = 0
    assert_equal   color(0, 0, 0, 1), pa.fill
    pa.fill   1
    assert_equal   color(1, 1, 1, 1), pa.fill
    pa.push fill: 0 do |_|
      assert_equal color(0, 0, 0, 1), pa.fill
    end
    assert_equal   color(1, 1, 1, 1), pa.fill
  end

  def test_stroke_accessor()
    pa = painter
    pa.stroke = 1
    assert_equal   color(1, 1, 1, 1), pa.stroke
    pa.stroke = 0
    assert_equal   color(0, 0, 0, 1), pa.stroke
    pa.stroke   1
    assert_equal   color(1, 1, 1, 1), pa.stroke
    pa.push stroke: 0 do |_|
      assert_equal color(0, 0, 0, 1), pa.stroke
    end
    assert_equal   color(1, 1, 1, 1), pa.stroke
  end

  def test_stroke_width_accessor()
    pa = painter
    assert_equal          0, pa.stroke_width
    pa.stroke_width =     1
    assert_equal          1, pa.stroke_width
    pa.stroke_width =     0
    assert_equal          0, pa.stroke_width
    pa.stroke_width       2
    assert_equal          2, pa.stroke_width
    pa.push stroke_width: 3 do |_|
      assert_equal        3, pa.stroke_width
    end
    assert_equal          2, pa.stroke_width
  end

  def test_stroke_cap_accessor()
    pa = painter
    assert_equal        :butt,   pa.stroke_cap
    pa.stroke_cap =     :round
    assert_equal        :round,  pa.stroke_cap
    pa.stroke_cap       :square
    assert_equal        :square, pa.stroke_cap
    pa.push stroke_cap: :butt do |_|
      assert_equal      :butt,   pa.stroke_cap
    end
    assert_equal        :square, pa.stroke_cap
    assert_raise(ArgumentError) {pa.stroke_cap :foo}
    assert_raise(ArgumentError) {pa.stroke_cap :BUTT}
    assert_raise(ArgumentError) {pa.stroke_cap Rays::CAP_BUTT}# ToDo: accept this
  end

  def test_stroke_join_accessor()
    pa = painter
    assert_equal         :miter,  pa.stroke_join
    pa.stroke_join =     :round
    assert_equal         :round,  pa.stroke_join
    pa.stroke_join       :square
    assert_equal         :square, pa.stroke_join
    pa.push stroke_join: :miter do |_|
      assert_equal       :miter,  pa.stroke_join
    end
    assert_equal         :square, pa.stroke_join
    assert_raise(ArgumentError) {pa.stroke_join :foo}
    assert_raise(ArgumentError) {pa.stroke_join :MITER}
    assert_raise(ArgumentError) {pa.stroke_join Rays::JOIN_MITER}# ToDo: accept this
  end

  def test_miter_limit_accessor()
    pa = painter
    assert_equal         2, pa.miter_limit
    pa.miter_limit =     3
    assert_equal         3, pa.miter_limit
    pa.miter_limit       4
    assert_equal         4, pa.miter_limit
    pa.push miter_limit: 9 do |_|
      assert_equal       9, pa.miter_limit
    end
    assert_equal         4, pa.miter_limit
  end

  def test_clip_accessor()
    pa = painter
    pa.clip = [1, 2, 3, 4]
    assert_equal [1, 2, 3, 4], pa.clip.to_a
    pa.clip = [5, 6, 7, 8]
    assert_equal [5, 6, 7, 8], pa.clip.to_a
    pa.clip    1, 2, 3, 4
    assert_equal [1, 2, 3, 4], pa.clip.to_a
    pa.push clip: [5, 6, 7, 8] do |_|
      assert_equal [5, 6, 7, 8], pa.clip.to_a
    end
    assert_equal [1, 2, 3, 4], pa.clip.to_a
  end

  def test_font_accessor()
    pa = painter
    f10, f20 = font(nil, 10), font(nil, 20)
    pa.font = f10
    assert_equal f10, pa.font
    pa.font = f20
    assert_equal f20, pa.font
    pa.font   f10
    assert_equal f10, pa.font
    pa.push font: f20 do |_|
      assert_equal f20, pa.font
    end
    assert_equal f10, pa.font
  end

  def test_font_name_size()
    pa = painter
    pa.font "Menlo", 10
    assert_equal "Menlo Regular", pa.font.name
    assert_equal 10, pa.font.size
    pa.font nil
    assert_not_equal "Menlo Regular", pa.font.name
    pa.font nil, 20
    assert_equal 20, pa.font.size
  end

  def test_color_by_name()
    pa = painter
    pa.fill =         :rgb001
    assert_equal color(0, 0, 1), pa.fill
    pa.fill =         [1, 0, 0]
    assert_equal color(1, 0, 0), pa.fill
    pa.fill            0, 1, 0
    assert_equal color(0, 1, 0), pa.fill
    pa.fill =         '#f00'
    assert_equal color(1, 0, 0), pa.fill
    pa.fill           '#0f0'
    assert_equal color(0, 1, 0), pa.fill
    pa.fill =         '#ff0000'
    assert_equal color(1, 0, 0), pa.fill
    pa.fill           '#00ff00'
    assert_equal color(0, 1, 0), pa.fill
  end

  def test_push()
    pa = painter
    pa.fill =           [1, 0, 0]
    assert_equal   color(1, 0, 0), pa.fill

    pa.push :all do |_|
      assert_equal color(1, 0, 0), pa.fill
      pa.fill =         [0, 1, 0]
      assert_equal color(0, 1, 0), pa.fill
    end
    assert_equal   color(1, 0, 0), pa.fill

    pa.push :state do |_|
      assert_equal color(1, 0, 0), pa.fill
      pa.fill =         [0, 1, 0]
      assert_equal color(0, 1, 0), pa.fill
    end
    assert_equal   color(1, 0, 0), pa.fill

    pa.push :matrix do |_|
      assert_equal color(1, 0, 0), pa.fill
      pa.fill =         [0, 1, 0]
      assert_equal color(0, 1, 0), pa.fill
    end
    assert_equal   color(0, 1, 0), pa.fill

    pa.push fill:       [0, 0, 1] do |_|
      assert_equal color(0, 0, 1), pa.fill
      pa.fill =         [1, 0, 0]
      assert_equal color(1, 0, 0), pa.fill
    end
    assert_equal   color(0, 1, 0), pa.fill

    pa.push stroke:     [0, 0, 1] do |_|
      assert_equal color(0, 1, 0), pa.fill
      pa.fill =         [1, 0, 0]
      assert_equal color(1, 0, 0), pa.fill
    end
    assert_equal   color(0, 1, 0), pa.fill
  end

  def test_shader()
    img = Rays::Image.new(10, 10).paint {
      shader "void main() {gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);}"
      fill   1, 0, 0
      rect   bounds
    }
    assert_equal [0.0, 0.0, 1.0, 1.0], img[0, 0].to_a
  end

end# TestPainter
