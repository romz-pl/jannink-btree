#ifndef ROMZ_JANNINK_KEY_H
#define ROMZ_JANNINK_KEY_H

// #include "totally_ordered.h"

//
// typedef int        Key;
// adapt key type to requirements
//

// int compare_keys(keyT key1, keyT key2);
//inline int compare_keys(Key key1, Key key2)
//{
//  return key1 - key2;        /* this is the case when keys are integer */
//}

class Key // : totally_ordered< Key >
{
public:
    Key();
    explicit Key( int value );

    static int compare( const Key& k1, const Key& k2 );

    bool operator< ( const Key& rhs ) const;
    bool operator==( const Key& rhs ) const;

    int get_value() const;

private:
    int m_value;
};


#endif




