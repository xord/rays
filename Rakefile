# -*- mode: ruby; coding: utf-8 -*-


%w[../xot ../rucy .]
  .map  {|s| File.expand_path "#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'rucy/rake'

require 'xot/module'
require 'rucy/module'
require 'rays/module'


MODULES     = [Xot, Rucy, Rays]
TESTS_ALONE = ['test/test_rays.rb']

use_external_library 'https://github.com/g-truc/glm',
  tag: '0.9.8.5',
  srcdir: 'NOSRC'

use_boost_library %w[core preprocessor mpl polygon],
  tag: 'boost-1.66.0'

build_native_library
build_ruby_extension
test_ruby_extension
generate_documents
build_ruby_gem

task :default => :test
