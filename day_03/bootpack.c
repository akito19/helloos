/* Call functions from external files. */

/* `_` がないと gcc compile 時にエラーが出た． */
extern void _io_hlt(void);

void HariMain(void)
{
fin:
    _io_hlt();
    goto fin;
}
