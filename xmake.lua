set_project("Template")
add_rules("plugin.vsxmake.autoupdate")
set_arch(os.arch())
add_rules("mode.debug","mode.release")
set_defaultmode("debug")

outputdir = ""
if is_mode("debug") then
	outputdir = "Debug-$(arch)"
elseif is_mode("release") then
	outputdir = "Release-$(arch)"
end


target("Core")
	set_kind("static")
	set_languages("c++23")
	set_runtimes("MT")

	set_targetdir("bin/".. outputdir .. "/Core")
	set_objectdir("bin-int/".. outputdir .. "/Core")

	--set_pcxxheader("Core/src/aepch.h")
	add_headerfiles("Core/src/**.h")
	add_files("Core/src/**.cpp")
	add_extrafiles("Core/vendor/**.inl")
	
	add_defines("_CRT_SECURE_NO_WARNINGS")

	add_includedirs(
		--"Core/vendor/spdlog/include", 
		--"Core/vendor/imgui",
		"Core/src", {public = true} )

	--add_deps("ImGui")
	add_links(
		--"ImGui",
		"dwmapi.lib",
		"user32.lib", "gdi32.lib","shell32.lib"	
	)

	if is_os("windows") then
		set_languages("c++23")
		add_defines("WINVER=0x0A00")
		add_defines("_WIN32_WINNT=0x0A00") 
		add_defines("CORE_PLATFORM_WINDOWS","CORE_BUILD_DLL")
		--add_defines("GLFW_INCLUDE_NONE")
	end

	-- after_build(function (target) 
	-- 	local outputdir = ""
	-- 	if is_mode("debug") then
	-- 		outputdir = "Debug-$(arch)"
	-- 	elseif is_mode("release") then
	-- 		outputdir = "Release-$(arch)"
	-- 	end
	-- 	print("hello")
    --     os.cp("bin/" .. outputdir.."/Aeat/Aeat.dll", path.join("bin/" .. outputdir.. "/Sandbox", path.basename("bin/" .. outputdir.."/Aeat/Aeat.dll") .. ".dll"))  -- Copy the built target to the destination
	-- end)

	if is_mode("debug") then
		set_runtimes("MTd")
		add_defines("CORE_DEBUG")
		set_symbols("debug")
	elseif is_mode("release") then 
		set_runtimes("MT")
		add_defines("CORE_RELEASE")
		set_optimize("fast")
	end


target("App")
	set_kind("binary")
	set_languages("c++26")
	set_runtimes("MT")

	set_targetdir("bin/".. outputdir .. "/App")
	set_objectdir("bin-int/".. outputdir .. "/App")

	add_headerfiles("App/src/**.h")
	add_files("App/src/**.cpp")

	
	--add_includedirs(
		--"Core/vendor/spdlog/include", 
		--"Core/src")
	add_deps("Core")
	add_links("Core")
	add_linkdirs("bin/"..outputdir.."/Core")



	if is_os("windows") then
		if is_mode("debug") then
			set_runtimes("MTd")
		elseif is_mode("release") then
			set_runtimes("MT")
		end
		add_defines("WINVER=0x0A00")
		add_defines("_WIN32_WINNT=0x0A00") 
		add_defines("CORE_PLATFORM_WINDOWS")
	end

	if is_mode("debug") then
		add_defines("CORE_DEBUG")
		set_symbols("debug")
	elseif is_mode("release") then 
		add_defines("CORE_RELEASE")
		set_optimize("fast")
	end
