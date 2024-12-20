/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021/08/19     bernard      the first version
 */

#include <rtthread.h>
#include <my_fs.h>
#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <dfs_romfs.h>
struct romfs_dirent _root_dirent[] =
{
    {ROMFS_DIRENT_DIR, "dev", RT_NULL, 0},
    {ROMFS_DIRENT_DIR, "mnt", RT_NULL, 0},
    {ROMFS_DIRENT_DIR, "proc", RT_NULL, 0},
    {ROMFS_DIRENT_DIR, "etc", RT_NULL, 0},
    {ROMFS_DIRENT_DIR, "bin", RT_NULL, 0},
    {ROMFS_DIRENT_FILE, "hello", ___userapps_apps_build_hello, sizeof(___userapps_apps_build_hello)},
};
const struct romfs_dirent romfs_root =
{
    ROMFS_DIRENT_DIR, "/", (rt_uint8_t *)_root_dirent, sizeof(_root_dirent) / sizeof(_root_dirent[0])
};

int mnt_init(void)
{
    //if (dfs_mount(RT_NULL, "/", "rom", 0, &(romfs_root)) == 0)
    //{
    //    rt_kprintf("ROM file system initializated!\n");
    //}

#if 1
    if (rt_device_find("virtio-blk0"))
    {
        /* mount virtio-blk as root directory */
        if (dfs_mount("virtio-blk0", "/", "ext", 0, RT_NULL) == 0)
        {
            rt_kprintf("file system initialization done ext!\n");
        }
        else
        {
            if (dfs_mount("virtio-blk0", "/", "elm", 0, RT_NULL) == 0)
            {
                rt_kprintf("file system initialization done elm!\n");
            }
            else
            {
                rt_kprintf("file system initialization fail!\n");
            }
        }
    } else {
        rt_kprintf("virtio-blk0 not found!\n");
    }
#endif
    return 0;
}
INIT_ENV_EXPORT(mnt_init);
#endif
