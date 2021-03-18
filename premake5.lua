workspace("CloneCraftGL")
	configurations({"Debug",  "Release"})
	platforms({"Win64", "Linux64"})
	architecture("x64")
	defines("UNICODE")
	location("build")
	--flags("OmitDefaultLibrary")

	includedirs({
		"includes",
	})
	
	libdirs({
		"lib",
		os.findlib("OpenGL"),
	})
	
	links({
		"Opengl32.lib",
		"glew32s.lib",
		"glfw3.lib",
	})

	project("CloneCraftGL")
	kind("ConsoleApp")
	language("C++")

		postbuildcommands({
			"{RMDIR} %{cfg.buildtarget.directory}textures",
			"{MKDIR} %{cfg.buildtarget.directory}textures",
			"{RMDIR} %{cfg.buildtarget.directory}shaders",
			"{MKDIR} %{cfg.buildtarget.directory}shaders",
			"{COPYDIR} ../textures %{prj.location}textures", -- for running with the debugger attached in VS
			"{COPYDIR} ../textures %{cfg.buildtarget.directory}textures", -- for running without the debugger attached
			"{COPYDIR} ../shaders %{prj.location}shaders", -- for running with the debugger attached in VS
			"{COPYDIR} ../shaders %{cfg.buildtarget.directory}shaders", -- for running without the debugger attached
		})

		files{
			"src/*.*",
			"textures/*.*",
			"shaders/*.*",
		}

		filter("platforms:Win64")
			system("windows")
			defines({
				"_WIN32"
			})
			
		filter("platforms:Linux64")
			system("linux")
			defines({
				"__linux__"
			})

		filter("configurations:Debug")
			defines({
			})
			links({
			})

		filter("configurations:Release")
			optimize("Full")
			defines({
				"NDEBUG"
			})
			links({
			})
