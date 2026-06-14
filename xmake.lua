set_project("SignEngine")
add_rules("plugin.vsxmake.autoupdate")
set_arch(os.arch())
add_rules("mode.debug","mode.release")
set_defaultmode("debug")
add_requires("directx-headers")

outputdir = ""
if is_mode("debug") then
	outputdir = "Debug-$(arch)"
elseif is_mode("release") then
	outputdir = "Release-$(arch)"
end


target("Sign")
	set_kind("static")
	set_languages("c++23")
	set_runtimes("MT")

	set_targetdir("bin/".. outputdir .. "/Sign")
	set_objectdir("bin-int/".. outputdir .. "/Sign")

	--set_pcxxheader("Core/src/aepch.h")
	add_headerfiles("Sign/src/**.h","d3dx12.h")
	add_files("Sign/src/**.cpp")
	add_extrafiles("Sign/vendor/**.inl")
	
	add_packages("directx-headers")
	add_defines("_CRT_SECURE_NO_WARNINGS")

	add_includedirs(
		--"Core/vendor/spdlog/include", 
		--"Core/vendor/imgui",
		"Sign/src", {public = true} )

	--add_deps("ImGui")
	add_links(
		--"ImGui",
		"dwmapi.lib", "d3d12.lib", "dxgi.lib",
		"d3dcompiler.lib", "dxguid.lib",
		"user32.lib", "gdi32.lib","shell32.lib"	
	)

	if is_os("windows") then
		set_languages("c++23")
		add_defines("WINVER=0x0A00")
		add_defines("_WIN32_WINNT=0x0A00") 
		add_defines("SIGN__PLATFORM_WINDOWS","SIGN__BUILD_DLL")
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
		add_defines("SIGN_DEBUG")
		set_symbols("debug")
	elseif is_mode("release") then 
		set_runtimes("MT")
		add_defines("SIGN_RELEASE")
		set_optimize("fast")
	end


target("App")
	set_kind("binary")
	set_languages("c++23")
	set_runtimes("MT")

	set_targetdir("bin/".. outputdir .. "/App")
	set_objectdir("bin-int/".. outputdir .. "/App")

	add_headerfiles("App/src/**.h")
	add_files("App/src/**.cpp")

	
	--add_includedirs(
		--"Core/vendor/spdlog/include", 
		--"Core/src")
	add_deps("Sign")
	add_links("Sign")
	add_linkdirs("bin/"..outputdir.."/Sign")



	if is_os("windows") then
		if is_mode("debug") then
			set_runtimes("MTd")
		elseif is_mode("release") then
			set_runtimes("MT")
		end
		add_defines("WINVER=0x0A00")
		add_defines("_WIN32_WINNT=0x0A00") 
		add_defines("SIGN_PLATFORM_WINDOWS")
	end

	if is_mode("debug") then
		add_defines("SIGN_DEBUG")
		set_symbols("debug")
	elseif is_mode("release") then 
		add_defines("SIGN_RELEASE")
		set_optimize("fast")
	end
