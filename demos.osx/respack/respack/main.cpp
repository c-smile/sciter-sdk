//
//  main.cpp
//  respack
//
//  Created by andrew on 2014-04-01.
//  Copyright (c) 2014 andrew fedoniouk. All rights reserved.
//

#include <iostream>
#include <map>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void convert(FILE* fin, FILE* fout, const char* name, int no, const char* id)
{
    int counter = 0;
    int line_counter = 0;
    
    fprintf(fout,"const byte %s_res_data_%d[] = {\n\t",id, no);
    while(!feof(fin)) {
      unsigned char c;
      fread( &c, 1,1, fin);
      fprintf(fout,"0x%02x",c);
      if( feof(fin) )
          break;
      fprintf(fout,",");
      ++counter;
      if(++line_counter > 40 ) {
        line_counter = 0;
        fprintf(fout,"\n");
      }
        
    }
    fprintf(fout," };\n");
    fprintf(fout,"%s_resource _resno_%d(\"%s\", %s_res_data_%d,%d);\n",id,no,name,id,no,counter);
}

typedef unsigned char byte;

const byte nn[] = { 0x01, 0x02, 0x03 };
const unsigned nn_length = 3;

struct resource
{
    static std::map<std::string, std::pair<const byte*, size_t>>& get_map() {
      static std::map<std::string, std::pair<const byte*, size_t>> _map; return _map;
    }
  static bool get( const char* name, const byte* &data, size_t& data_length) {
      auto it = get_map().find(std::string(name));
      if( it == get_map().end() )
          return false;
      data = (*it).second.first ;
      data_length = (*it).second.second;
      return true;
  }
  resource( const char* name, const byte* data, size_t data_length ) {
     get_map()[ std::string(name) ] = std::pair<const byte*, size_t>( data, data_length );
  }
};

resource nn_res("name",nn,nn_length);


void process_folder(FILE* fout, const char* path, int& no, const char* id) {
    
    DIR *dp;
    struct dirent *dirp;
    struct stat filestat;
    
    std::string dir(path);
    
    dp = opendir( path );
    if (dp == nullptr)
    {
        std::cerr << "error(" << errno << ") opening " << dir << " folder" << std::endl;
        return;
    }
    
    while ((dirp = readdir( dp )))
    {
        std::string filename = dirp->d_name;
        if(filename[0] == '.')
            continue;
        std::string filepath = dir + "/" + filename;
        
        // If the file is a directory (or is in some way invalid) we'll skip it
        if (stat( filepath.c_str(), &filestat )) continue;
        if (S_ISDIR( filestat.st_mode ))         continue;
        
        // Endeavor to read a single number from the file and display it
        FILE* fin = fopen(filepath.c_str(), "rb");
        if( !fin ) {
          std::cerr << "error(" << errno << ") reading " << filepath << " file" << std::endl;
          continue;
        }
        
        convert(fin, fout, filename.c_str(), ++no, id);
        
        std::cout << "processed:" << filepath << std::endl;
        
        fclose(fin);
    }
    closedir( dp );
}



int main(int argc, const char * argv[])
{
    if( argc < 3  ) {
      std::cerr << argv[0] << "\n";
        
      std::cerr << "respack will encode all files in the folder as C++ literals.\n";
      std::cerr << "Usage: respack resfilename resfolder [id]\n";
      exit(0);
    }
    
    int n = 0;
    
    FILE* fout = fopen(argv[1], "w");
    if( !fout ) {
        std::cerr << "error(" << errno << ") opening " << argv[1] << " file for writing" << std::endl;
        return errno;
    }
    
    const char* id = "";
    if( argc >= 4 )
        id = argv[3];
    
    const char* preambula =
    "#include <map>\n"
    "#include <string>\n"    
    "typedef unsigned char byte;\n"
    "struct %s_resource\n"
    "{\n"
    "    static std::map<std::string, std::pair<const byte*, size_t>>& get_map() {\n"
    "        static std::map<std::string, std::pair<const byte*, size_t>> _map; return _map;\n"
    "    }\n"
    "    static bool get( const char* name, const byte* &data, size_t& data_length) {\n"
    "        auto it = get_map().find(std::string(name));\n"
    "        if( it == get_map().end() )\n"
    "            return false;\n"
    "        data = (*it).second.first ;\n"
    "        data_length = (*it).second.second;\n"
    "        return true;\n"
    "    }\n"
    "    %s_resource( const char* name, const byte* data, size_t data_length ) {\n"
    "        get_map()[ std::string(name) ] = std::pair<const byte*, size_t>( data, data_length );\n"
    "    }\n"
    "};\n"
    "bool get_%s_resource(const char* name, const byte* &data, size_t& data_length) { return %s_resource::get(name,data,data_length); }\n";

    fprintf(fout, preambula, id, id, id, id);
    
    process_folder(fout, argv[2], n, id);
    
    fclose(fout);
    
    //const char16_t str[] = u"hello world";
    //std::string16 s;
    // insert code here...
    //std::cout << argv[0] << " Hello, World!\n";

    //const byte* t;
    //size_t      t_length;
    
    //if(resource::get("name",t,t_length))
    //  std::cout << "found!\n";
    
    return 0;
}

