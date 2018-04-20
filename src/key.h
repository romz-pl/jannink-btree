#ifndef ROMZ_JANNINK_KEY_H
#define ROMZ_JANNINK_KEY_H

//
// Represents key
//

class Key
{
public:
    explicit Key( int value );

    static int compare( const Key& k1, const Key& k2 );

    bool operator< ( const Key& rhs ) const;
    bool operator==( const Key& rhs ) const;

    int get_value() const;

private:
    int m_value;
};


#endif




