# A very hacky first attemt at a GUnit guard
# TODO: guard a cmakelist
# TODO: add better messages to guard

require 'active_support/inflector'
require 'guard/guard'


module ::Guard
  class GTest < Guard

    def initialize(w = [], options={})
      super
      @options = {
                :all_on_start   => true,
                :all_after_pass => true,
                :keep_failed    => true,
                :auto_make       => true,
                :test_path      => 'bin/test'
      }.update(options)


      @last_failed  = false
      @failed_paths = []
    end

    def start
      run_all if @options[:all_on_start]
    end

    def run_all
      paths = _test_paths

      ::Guard::UI.info(options[:message] || "Running: #{paths.join(' ')}", :reset => true)

      return true if paths.empty?

      passed = _run(paths)

      @failed_paths = [] if passed
      @last_failed  = !passed
    end

    def reload
      @failed_paths = []
    end

    def run_on_change(paths)
      paths += @failed_paths.uniq if @options[:keep_failed]
      passed = _run(paths)

      if passed
        # clean failed paths memory
        @failed_paths -= paths if @options[:keep_failed]

        # run all the tests if the changed tests failed, like autotest
        run_all if @last_failed && @options[:all_after_pass]
      else
        # remember failed paths for the next change
        @failed_paths += paths if @options[:keep_failed]

        # track whether the changed tests failed for the next change
        @last_failed = true
      end
    end

    private
    def _run(paths)
      if @options[:auto_make]
        unless system('make')
          ::Guard::Notifier.notify(
                                   "Make failed!", #TODO: Add why it failed
                                   :title => "Make Failed",
                                   :image =>  :failed
                                   )
          return false
        end
      end

      # TODO: Remove this
      ::Guard::UI.info("Running: #{paths.join(' ')}", :reset => true)
      if !system(paths.map{|x| './' + x}.join(' ')) #TODO: Notify on failure
        ::Guard::Notifier.notify(
                                 "Test #{paths.join(' ')} failed!", #TODO: Add why it failed
                                 :title => "Test Failed",
                                 :image =>  :failed
                                 )
      else @last_failed #TODO: Move this somewhere else
        ::Guard::Notifier.notify(
                                 "Suceeded!", #TODO: Add why/what worked
                                 :title => "Test Passed",
                                 :image =>  :success
                                 )

      end

      true
    end

    def _test_paths
      @paths ||= (Dir[File.join(@options[:test_path], '**', '*_test')]).uniq.compact
    end
  end
end

guard 'gtest', :test_paths => ['bin/test'] do
  watch(%r{(((?!\/).)+)\.(h|c|hxx|cxx|cpp)$}) { |m|
    name = m[1].underscore

    if name =~ /_test$/
      "bin/test/#{name}"
    else
      "bin/test/#{name}_test"
    end
  }

  # Enable this if not using automake
  watch(%r{(.+)_test$}) {|m| "#{m[1]}_test" }

  # Enable this if using automake The top one covers this case
  #watch(%r{(((?!\/).)+_test)\.cxx$}) {|m| "bin/test/#{m[1]}"}
end
