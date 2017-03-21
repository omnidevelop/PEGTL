// Copyright (c) 2014-2017 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAOCPP_PEGTL_INCLUDE_INTERNAL_FILE_MAPPER_HH
#define TAOCPP_PEGTL_INCLUDE_INTERNAL_FILE_MAPPER_HH

#include <unistd.h>
#include <sys/mman.h>

#include "../config.hh"

#include "file_opener.hh"

#include "../input_error.hh"

namespace TAOCPP_PEGTL_NAMESPACE
{
   namespace internal
   {
      class file_mapper
      {
      public:
         explicit
         file_mapper( const std::string & filename )
               : file_mapper( file_opener( filename ) )
         { }

         explicit
         file_mapper( const file_opener & reader )
               : m_size( reader.size() ),
                 m_data( static_cast< const char * >( ::mmap( nullptr, m_size, PROT_READ, MAP_FILE | MAP_PRIVATE, reader.m_fd, 0 ) ) )
         {
            if ( m_size && ( intptr_t( m_data ) == -1 ) ) {
               TAOCPP_PEGTL_THROW_INPUT_ERROR( "unable to mmap() file " << reader.m_source << " descriptor " << reader.m_fd );
            }
         }

         ~file_mapper()
         {
            ::munmap( const_cast< char * >( m_data ), m_size );  // Legacy C interface requires pointer-to-mutable but does not write through the pointer.
         }

         file_mapper( const file_mapper & ) = delete;
         void operator= ( const file_mapper & ) = delete;

         bool empty() const
         {
            return m_size == 0;
         }

         std::size_t size() const
         {
            return m_size;
         }

         using iterator = const char *;
         using const_iterator = const char *;

         iterator data() const
         {
            return m_data;
         }

         iterator begin() const
         {
            return m_data;
         }

         iterator end() const
         {
            return m_data + m_size;
         }

         std::string string() const
         {
            return std::string( m_data, m_size );
         }

      private:
         const std::size_t m_size;
         const char * const m_data;
      };

   } // namespace internal

} // namespace TAOCPP_PEGTL_NAMESPACE

#endif