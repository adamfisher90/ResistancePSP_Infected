/*
  Remastered Controls: Resistance
  Copyright (C) 2018, TheFloW

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdisplay.h>

#include <stdio.h>
#include <string.h>

#include <systemctrl.h>

PSP_MODULE_INFO("ResistanceRemastered", 0x1007, 1, 0);

#define FAKE_DEVNAME      "usbpspcm0:"
#define FAKE_UID          0x12345678

static STMOD_HANDLER previous;
static int init_mode = 0;

int sceIoReadPatched(SceUID fd, void *data, SceSize size) {
  int k1 = pspSdkSetK1(0);

  // Check if it's the fake UID
  if (fd == FAKE_UID) {
    // Wait to relieve thread, since sceIoRead on usbpspcm0: is actually a blocking function
    sceDisplayWaitVblankStart();

    int len = 0;

    if (init_mode == 0) {
      // Don't Activate Resistance Plus
      sprintf(data, "%1d%1d", 1, 0);
      len = 3;
      init_mode++;
    } else if (init_mode == 1) {
      // Activate infected mode
      sprintf(data, "%1d%1d", 2, 1);
      len = 3;
      init_mode++;
    }

    pspSdkSetK1(k1);
    return len;
  }

  pspSdkSetK1(k1);
  return sceIoRead(fd, data, size);
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode) {
  int k1 = pspSdkSetK1(0);

  // Fake UID
  if (strcmp(file, FAKE_DEVNAME) == 0) {
    pspSdkSetK1(k1);
    return FAKE_UID;
  }

  pspSdkSetK1(k1);
  return sceIoOpen(file, flags, mode);
}

int sceIoWritePatched(SceUID fd, const void *data, SceSize size) {
  int k1 = pspSdkSetK1(0);

  // Fake success
  if (fd == FAKE_UID) {
    pspSdkSetK1(k1);
    return size;
  }

  pspSdkSetK1(k1);
  return sceIoWrite(fd, data, size);
}

int sceIoClosePatched(SceUID fd) {
  int k1 = pspSdkSetK1(0);

  // Fake success
  if (fd == FAKE_UID) {
    pspSdkSetK1(k1);
    return 0;
  }

  pspSdkSetK1(k1);
  return sceIoClose(fd);
}

int sceIoDevctlPatched(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen) {
  int k1 = pspSdkSetK1(0);

  if (cmd == 0x03415001) { // Fake connection for register
    u32 conn[2];
    conn[0] = 0;
    conn[1] = 0x81;
    int res = sceKernelStartThread(*(u32 *)indata, sizeof(conn), &conn);
    pspSdkSetK1(k1);
    return res;
  } else if (cmd == 0x03415002) { // Fake success for unregister
    pspSdkSetK1(k1);
    return 0;
  } else if (cmd == 0x03435005) { // Fake devname for bind
    strcpy(outdata, FAKE_DEVNAME);
    pspSdkSetK1(k1);
    return 0;
  }

  pspSdkSetK1(k1);
  return sceIoDevctl(dev, cmd, indata, inlen, outdata, outlen);
}

int sceUsbStartPatched(const char *driverName, int size, void *args) {
  return 0;
}

int sceUsbStopPatched(const char *driverName, int size, void *args) {
  return 0;
}

int sceUsbActivatePatched(u32 pid) {
  return 0;
}

int sceUsbDeactivatePatched(u32 pid) {
  return 0;
}

int OnModuleStart(SceModule *mod) {
  if (strcmp(mod->modname, "Resistance") == 0) {

    // Redirect IO functions to fake usbpspcm0: communication
    sctrlHookImportByNID(mod, "IoFileMgrForUser", 0x109F50BC, sceIoOpenPatched);
    sctrlHookImportByNID(mod, "IoFileMgrForUser", 0x6A638D83, sceIoReadPatched);
    sctrlHookImportByNID(mod, "IoFileMgrForUser", 0x42EC03AC, sceIoWritePatched);
    sctrlHookImportByNID(mod, "IoFileMgrForUser", 0x810C4BC3, sceIoClosePatched);
    sctrlHookImportByNID(mod, "IoFileMgrForUser", 0x54F5FB11, sceIoDevctlPatched);

    // Redirect USB functions to fake success
    sctrlHookImportByNID(mod, "sceUsb", 0xAE5DE6AF, sceUsbStartPatched);
    sctrlHookImportByNID(mod, "sceUsb", 0xC2464FA0, sceUsbStopPatched);
    sctrlHookImportByNID(mod, "sceUsb", 0x586DB82C, sceUsbActivatePatched);
    sctrlHookImportByNID(mod, "sceUsb", 0xC572A9C8, sceUsbDeactivatePatched);

    // Clear caches
    sceKernelDcacheWritebackAll();
    sceKernelIcacheClearAll();
  }

  if (!previous)
    return 0;

  return previous(mod);
}

int module_start(SceSize args, void *argp) {
  previous = sctrlHENSetStartModuleHandler(OnModuleStart);
  return 0;
}
