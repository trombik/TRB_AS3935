desc "Generate doxygen XML"
task :doxygen => [:install_moxygen] do
  sh "doxygen .Doxyfile"
end

desc "Generate markdown document from doxygen XML"
task :md => [:doxygen] do
  Dir.mkdir("docs/md") unless Dir.exist?("docs/md")
  sh "node vendor/node_modules/moxygen/bin/moxygen.js --output docs/md/README.md docs/xml"
end

desc "Install moxygen to current directory"
task :install_moxygen do
  sh "npm install --prefix vendor moxygen"
end
