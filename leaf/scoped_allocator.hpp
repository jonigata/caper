#ifndef SCOPED_ALLOCATOR_HPP
#define SCOPED_ALLOCATOR_HPP

#include <new>
#include <iostream>

template < class PageProvider >
class scoped_allocator {
private:
		class base_klass {
		public:
				base_klass( base_klass* next ) : next_(next) {}
				virtual ~base_klass() {}
				virtual void destruct() = 0;
				base_klass* next() { return next_; }
		private:
				base_klass* next_;
		};

		template < class T >
		class klass : public base_klass {
		public:
				klass( base_klass* next ) : base_klass( next ) {}
				~klass() {}
				T* address() { return (T*)t_; }
				void destruct() { (*(T*)t_).~T(); }
		private:
				char t_[sizeof(T)];
		};

		struct page_header {
				page_header* next;
		};

public:
		scoped_allocator( PageProvider& pp	) : pp_( pp )
		{
				page_ = NULL;
				first_ = NULL;
				allocate_page();
		}
		~scoped_allocator()
		{
				base_klass* k = first_;
				while( k ) {
						base_klass* nk = k->next();
						k->destruct();
						k = nk;
				}

				page_header* p = page_;
				while( p ) {
						page_header* n = p->next;
						pp_.deallocate( (char*)p );
						p = n;
				}
		}

		template < class T >
		T* allocate() { return new ( allocate_aux<T>()->address() ) T; }

		template < class T, class A0 >
		T* allocate( const A0& a0 ) { return new ( allocate_aux<T>()->address() ) T( a0 ); }

		template < class T, class A0, class A1 >
		T* allocate( const A0& a0, const A1& a1 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1 );
		}
		
		template < class T, class A0, class A1, class A2 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2 );
		}

		template < class T, class A0, class A1, class A2, class A3 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5, a6 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5, a6, a7 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5, a6, a7, a8 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10 )
		{
				return new ( allocate_aux<T>()->address() ) T( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 );
		}

private:
		template < class T >
		klass<T>* allocate_aux()
		{
				c_ += sizeof( klass<T> );
				if( e_ < c_ ) {
						allocate_page();
				}
				klass<T>* kt = new klass<T>( first_ );
				first_ = kt;
				return kt;
		}

		void allocate_page()
		{
				pp_.allocate( b_, e_ );
				c_ = b_;

				page_header* page = (page_header*)b_;
				page->next = page_;
				page_ = page;
		}
		
private:
		PageProvider& pp_;
		page_header* page_;
		base_klass* first_;
		char* b_;
		char* c_;
		char* e_;
};

class local_page_provider {
public:
		local_page_provider( void* b, void* e ) : b_( (char*)b ), e_( (char*)e ) {}
		~local_page_provider() {}

		bool allocate( char*& b, char*& e ) { bool f = b_ ? true : false; b = b_; e = e_; b_ = e_ = NULL; return f; }
		void deallocate( char* ) { }

private:
		char* b_;
		char* e_;

};

class heap_page_provider {
public:
		heap_page_provider( size_t size ) : size_( size ) {}
		~heap_page_provider() {}

		bool allocate( char*& b, char*& e ) { b = new char[size_]; e = b + size_; return b ? true : false; }
		void deallocate( char* p ) { delete [] p; }

private:
		size_t size_;
		
};

class local_cage {
public:
		local_cage( char* b, char* e ) : lpp_( b, e ), sa_( lpp_ ) { }
		~local_cage(){}

		template < class T >
		T* allocate() { return sa_.allocate<T>(); }

		template < class T, class A0 >
		T* allocate( const A0& a0 ) { return sa_.allocate<T>( a0 ); }

		template < class T, class A0, class A1 >
		T* allocate( const A0& a0, const A1& a1 )
		{
				return sa_.allocate<T>( a0, a1 );
		}
		
		template < class T, class A0, class A1, class A2 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2 )
		{
				return sa_.allocate<T>( a0, a1, a2 );
		}

		template < class T, class A0, class A1, class A2, class A3 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 );
		}

private:
		local_page_provider						lpp_;
		scoped_allocator< local_page_provider > sa_;

};

class heap_cage {
public:
		heap_cage( size_t size = 16384 ) : hpp_( size ), sa_( hpp_ ) {}
		~heap_cage() {}

		template < class T >
		T* allocate() { return sa_.allocate<T>(); }

		template < class T, class A0 >
		T* allocate( const A0& a0 ) { return sa_.allocate<T>( a0 ); }

		template < class T, class A0, class A1 >
		T* allocate( const A0& a0, const A1& a1 )
		{
				return sa_.allocate<T>( a0, a1 );
		}
		
		template < class T, class A0, class A1, class A2 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2 )
		{
				return sa_.allocate<T>( a0, a1, a2 );
		}

		template < class T, class A0, class A1, class A2, class A3 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 );
		}

		template < class T, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10 >
		T* allocate( const A0& a0, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10 )
		{
				return sa_.allocate<T>( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 );
		}

private:
		heap_page_provider					   hpp_;
		scoped_allocator< heap_page_provider > sa_;
					  
};

#endif // SCOPED_ALLOCATOR_HPP
