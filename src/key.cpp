#include "key.h"

//
//
//
Key::Key( ) : m_value( 0 )
{

}

//
//
//
Key::Key( int value ) : m_value( value )
{

}

//
//
//
int Key::compare( const Key& k1, const Key& k2 )
{
    return ( k1.m_value - k2.m_value );
}

//
//
//
int Key::get_value() const
{
    return m_value;
}

//
//
//
bool Key::operator< ( const Key& rhs ) const
{
    return ( m_value < rhs.m_value );
}

//
//
//
bool Key::operator==( const Key& rhs ) const
{
    return ( m_value == rhs.m_value );
}
