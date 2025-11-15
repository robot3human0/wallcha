//
// Created by robot3human0 on 15/11/25.
//

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

using StrList = std::vector< std::string >;

static StrList image_mime_types{ ".jpg", ".jpeg", ".bmp", ".png" };

StrList collect_images( const std::string &path )
{
    StrList images;
    for ( const auto &entry : fs::recursive_directory_iterator( path ) )
    {
        if ( !entry.is_regular_file() )
        {
            continue;
        }

        auto ext = entry.path().extension().string();
        for ( auto &c : ext )
            c = static_cast< char >( tolower( c ) );

        bool is_ext_allowed = std::any_of( image_mime_types.begin(), image_mime_types.end(),
                                           [&ext]( const std::string &ae ) { return ae == ext; } );

        if ( is_ext_allowed )
        {
            images.push_back( entry.path().string() );
        }
    }

    return images;
}

void set_wallpaper( const std::string &path )
{
    const std::string cmd = "xfce4-set-wallpaper " + path;
    system( cmd.c_str() );
}

int main( int argc, char **argv )
{
    if ( argc != 3 )
    {
        std::cerr << "Usage: " << argv[0] << " <directory> <interval_minutes>" << std::endl;
        return 1;
    }

    const std::string path     = argv[1];
    const int interval_minutes = std::stoi( argv[2] );

    if ( !fs::exists( path ) )
    {
        std::cerr << "Directory " << path << " does not exist" << std::endl;
        return 1;
    }

    if ( interval_minutes <= 0 )
    {
        std::cerr << "The interval " << interval_minutes << " must be greater than 0" << std::endl;
        return 1;
    }

    constexpr int day_minutes = 60 * 24;
    if ( interval_minutes > day_minutes )
    {
        std::cerr << "The interval " << interval_minutes << " must be less than " << day_minutes << std::endl;
        return 1;
    }

    const auto images = collect_images( path );
    if ( images.empty() )
    {
        std::cerr << "No images detected" << std::endl;
        return 1;
    }

    const pid_t pid = fork();
    if ( pid < 0 ) return 1;
    if ( pid > 0 ) return 0;

    size_t idx{};
    while ( true )
    {
        set_wallpaper( images[idx] );
        idx = ( idx + 1 ) % images.size();
        std::this_thread::sleep_for( std::chrono::minutes( interval_minutes ) );
    }

    return 0;
}
