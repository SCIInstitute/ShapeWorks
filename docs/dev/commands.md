# How to Add ShapeWorks Commands?

To add ShapeWorks Commands, the following steps need to be followed:

1. Declare command in Applications/shapeworks/Commands.h

    ``
    COMMAND_DECLARE(CommandName, CommandType);
    ``

    CommandName is the name of the command.  
    CommandType is the type of command.  
    ShapeWorks supports ImageCommand, MeshCommand, ParticleSystemCommand, OptimizeCommandGroup and GroomCommandGroup  

2. Define command in  
    + Applications/shapeworks/ImageCommands.cpp if CommandType is ImageCommand  
    + Applications/shapeworks/MeshCommands.cpp if CommandType is MeshCommand  
    + Applications/shapeworks/ParticleSystemCommands.cpp if CommandType is ParticleSystemCommand  
    + Applications/shapeworks/Commands.cpp for other types of commands  

    ```
    ///////////////////////////////////////////////////////////////////////////////
    // CommandName
    ///////////////////////////////////////////////////////////////////////////////
    void CommandName::buildParser()
    {
      const std::string prog = "commandname"; // supports command-name as well
      const std::string desc = "brief description of command";
      parser.prog(prog).description(desc);

      parser.add_option("--optionname").action("store").type("double").set_default(0.01).help("Description of optionname.");
      // additional options/arguments... 
        
      Command::buildParser();
    }

    bool CommandName::execute(const optparse::Values &options, SharedCommandData &sharedData)
    {
      if (!sharedData.validImage())
      {
        std::cerr << "No image to operate on\n";
        return false;
      }

      double optionName = static_cast<double>(options.get("optionname"));
      // read additional options/arguments... 

      sharedData.image.example(optionName, ...); // call related function
      return true;
    }
    ```

3. Add command to Applications/shapeworks/shapeworks.cpp

    ``
    shapeworks.addCommand(CommandName::getCommand());
    ``
