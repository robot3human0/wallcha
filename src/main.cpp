//
// Created by robot3human0 on 15/11/25.
//

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#define MAJOR_VERSION 2
#define MINOR_VERSION 0

namespace
{

constexpr int kDefaultInterval = 30;

using StrList = std::vector< std::string >;

StrList image_mime_types{ ".jpg", ".jpeg", ".bmp", ".png" };

const char* short_opts = "d:c:hv";

const option long_opts[] = {
    { "directory", required_argument, nullptr, 'd' },
    { "count",     required_argument, nullptr, 'c' },
    { "help",      no_argument,       nullptr, 'h' },
    { "version",   no_argument,       nullptr, 'v' },
    { nullptr,     0,                 nullptr,  0  }
};

void help( const std::string &project_name )
{
    std::cout << "Usage: " << project_name << " -d <path/to/directory> -c <minutes>\n"
                             "Options:\n"
                             "  -d, --directory  Path to the wallpaper directory.\n"
                             "  -c, --count      Interval between wallpaper changes in minutes,\n"
                             "                   if not provided, default interval 30 min.\n"
                             "  -h, --help       Show this help.\n"
                             "  -v, --version    Show version.\n"
                ;
}

} // end unnamed namespace

StrList collect_images( const std::string &path )
{
    StrList images;
    for ( const auto &entry : std::filesystem::recursive_directory_iterator( path ) )
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
    const std::string cmd = "xfce4-set-wallpaper \"" + path + "\"";
    system( cmd.c_str() );
}

int main( int argc, char **argv )
{
    if ( argc == 1 )
    {
        help( argv[0] );
        return 1;
    }

    std::string path {};
    int interval_minutes {};

    while ( true )
    {
        int opt = getopt_long( argc, argv, short_opts, long_opts, nullptr );

        if ( opt == -1 ) break;

        switch ( opt )
        {
            case 'd':
                path = optarg;
                break;
            case 'c':
                interval_minutes = std::stoi( optarg );
                break;
            case 'h':
                help( argv[0] );
                return 0;
            case 'v':
                std::cout << "Version: " << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;
                return 0;
            default:
                std::cerr << "Unknown option: " << opt << std::endl;
                return 1;
        }
    }

    if ( !std::filesystem::exists( path ) )
    {
        std::cerr << "Directory " << path << " does not exist" << std::endl;
        return 1;
    }

    if ( interval_minutes < 0 )
    {
        std::cerr << "The interval " << interval_minutes << " must be greater than 0" << std::endl;
        return 1;
    }
    else if ( interval_minutes == 0 )
    {
        interval_minutes = kDefaultInterval;
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
