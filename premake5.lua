workspace("CloneCraftGL")
	configurations({"Debug",  "Release"})
	platforms({"Win64"})
	architecture("x64")
	defines("UNICODE")
	location("./")
	--flags("OmitDefaultLibrary")

	includedirs({
		"$(BOOST_SDK)",
		"dependencies/includes",
	})
	
	libdirs({
		"dependencies/lib",
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
			"{RMDIR} %{cfg.buildtarget.directory}res",
			"{MKDIR} %{cfg.buildtarget.directory}res",
			"{COPYDIR} res %{cfg.buildtarget.directory}res",
		})

		files{
			"src/*.*",
			"res/textures/*.*",
			"res/shaders/*.*",
			"res/models/blocks/*.*",
		}

		filter("platforms:Win64")
			system("windows")
			defines({
				"_WIN32"
			})

		filter("configurations:Debug")
			defines({
			})
			links({
				"libnoise-d.lib",
			})

		filter("configurations:Release")
			optimize("Full")
			defines({
				"NDEBUG"
			})
			links({
				"libnoise-r.lib",
			})
