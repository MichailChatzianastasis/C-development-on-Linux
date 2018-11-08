{
    printk( KERN_NOTICE "Simple-driver: Device file is read at offset = %i, read bytes count = %u"
                , (int)*position
                , (unsigned int)count );
    /* If position is behind the end of a file we have nothing to read */
    if( *position >= g_s_Hello_World_size )
        return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
    if( *position + count > g_s_Hello_World_size )
        count = g_s_Hello_World_size - *position;
    if( copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0 )
        return -EFAULT;    
    /* Move reading position */
    *position += count;
    return count;
}

