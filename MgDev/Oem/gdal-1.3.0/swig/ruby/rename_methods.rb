
class SwigRename
  @@keywords = %w{new create delete union}

  # define structures
  RenameStruct = Struct.new(:class_name, :original_method_name, :new_method_name)
  AliasStruct = Struct.new(:class_name, :method_name, :alias_name)

  def initialize(filename)
    @filename = filename
    @renames = Array.new
    @aliases = Array.new
  end
  
  def convert_file()
    begin
      file = File.open(@filename, 'r')
      process_file(file)
      print_output()
    ensure
  		file.close
    end
  end

  private 
  
  def process_file(file)
	  while line = file.gets
      line = line.strip!

      next if line == ''
    
      if line.match(/SWIGTYPE_p_(.*),/)
      then
	      class_name = $1
   	 	  next
		  end

		  next unless line.match(/^(rb_define_method|rb_define_module_function)/)
		  method_type = $1
  	
      match = line.match(/.*"(.*)"/)
     	original_method_name = $1

      # Eliminate methods that don't start with a
      # capital letter.
      next if original_method_name =~ /^[a-z]/ 

		  # Eliminate = at the end since SWIG will
			# barf on it.  This seems to work okay,
	  	# because SWIG will apply a rename
	  	# directive to the method anyway.
	  	next if original_method_name =~ /(.*)=$/
      
      rubified_method_name = rubify_method_name(original_method_name)
      alias_name = check_alias(rubified_method_name)

			new_method_name = (alias_name.nil?) ? rubified_method_name: alias_name      

			
			if method_type == 'rb_define_method'
			  temp_class = class_name
			else
			  temp_class = nil
			end
			
      @renames.push(RenameStruct.new(temp_class, original_method_name, new_method_name))
            
		  if not alias_name.nil?
		    @aliases.push(AliasStruct.new(temp_class, new_method_name, rubified_method_name))
			end
		end
  end

  def rubify_method_name(method_name)
	  new_method_name = method_name.gsub(/([[:lower:]])([[:upper:]])/, '\1_\2')
	  new_method_name.downcase!
  end

	def check_alias(method_name)		  		  
 	  # If the method name is a C++ or SWIG keyword?  If so,
 	  # add an underscore to the end.
 	  result = nil
 	  
	  if @@keywords.include?(method_name)
	    result = method_name += "_"
		end

		return result
	end
	
		
  def print_output
    puts "// -------  SWIG Renames ------------"

    @renames.each do |struct|
      # Add in the class name first
      STDOUT.write "%rename(#{struct.new_method_name}) "
      STDOUT.write "#{struct.class_name}::" if not struct.class_name.nil?
      STDOUT.write "#{struct.original_method_name}"
      puts ";"
	  end
  
	  puts ""
	  puts ""

	  puts "// -------  SWIG Aliases ------------"
	  @aliases.each do |struct|
      STDOUT.write "%alias "
      STDOUT.write "#{struct.class_name}::" if not struct.class_name.nil?
      STDOUT.write "#{struct.method_name} "
      STDOUT.write "\"#{struct.alias_name}\""
      puts ";"
		end

	  puts ""
	  puts ""
	end
end
				    

Dir.new(Dir.getwd)
Dir.glob('*.cpp') do |file|
  path = File.join(Dir.getwd, file)
  rename = SwigRename.new(path)	
  rename.convert_file()
end

  