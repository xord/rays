# -*- coding: utf-8 -*-


%w[../xot ../rucy .]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'mkmf'
require 'xot/extconf'
require 'xot/module'
require 'rucy/module'
require 'rays/module'


Xot::ExtConf.new Xot, Rucy, Rays do
  setup do
    headers    << 'ruby.h'
    local_libs << 'rucy'
    libs.unshift 'gdi21', 'opengl32' if win32?
    frameworks << 'AppKit' << 'OpenGL' if osx?
    $LDFLAGS   << ' -Wl,--out-implib=native.dll.a' if cygwin?
  end

  create_makefile 'rays/native'
end
