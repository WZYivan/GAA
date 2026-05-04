set_project("GAA")
set_version("1.0.0")

option("strict", {showmenu=true, default=true, description="Enable strict warnings for compiler"})
option("test", {showmenu=true, default=true, description="Enable build test"})
option("dev", {showmenu=true, default=false, description="Enable build dev (for developers)"})

add_requires("eigen", "boost")
add_rules("mode.debug", "mode.release")
set_languages("c++23", "c23")

target("gaa")
set_kind("shared")
add_files("src/gaa/**/*.cpp")
add_includedirs("include", { public = true })
add_packages("eigen", "boost", { public = true })
if is_config("strict") then
    add_cxxflags("-Wall", "-Wextra", "-pedantic", "-Werror")
end

if is_config("test") then
    local test_files = os.files("test/*.cpp")
    for _, filepath in ipairs(test_files) do 
        local name = path.basename(filepath):gsub("%.cpp$", "")
        target(name)
        set_kind("binary")
        set_group("test")
        add_deps("gaa")
        add_files(filepath)

        add_tests(name, { run = name } )
    end
end

if is_config("dev") then
    local test_files = os.files("dev/**.cpp")
    if #test_files == 0 then
        print("warning: no files found in dev/**/*.cpp")
    end
    for _, filepath in ipairs(test_files) do 
        local name = path.basename(filepath):gsub("%.cpp$", "")
        target(name)
            set_kind("binary")
            set_group("dev")
            add_deps("gaa")
            add_files(filepath)
    end
end
