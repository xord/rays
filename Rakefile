# -*- mode: ruby; coding: utf-8 -*-


%w[../xot ../rucy .]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'xot/rake'
require 'xot/module'
require 'rucy/module'
require 'rays/module'

include Xot::Rake


MODULES     = [Xot, Rucy, Rays].map {|m| m.const_get :Module}
MODULE      = MODULES.last
TESTS_ALONE = ['test/test_rays.rb']


task :default => :build

task :build => :ext


MODULES.each {|m| m.load_tasks :lib, :ext, :test, :doc, :gem}
