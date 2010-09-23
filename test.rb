#!/usr/bin/ruby

require 'V8/V8'

puts V8.compile('this_works.js')
puts V8.compile('this_works_too.js')
puts V8.compile('this_fails.js')
puts V8.compile('this_works.js')
