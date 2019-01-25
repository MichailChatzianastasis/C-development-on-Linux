# C-Development-on-Linux

<h1>  Low level programming in language C for the course Operating System in National Technical University of Athens. </h1>

 
 <div>
 
<h2>Kernel Drivers  Development</h2>


Assigment1:
Implementation of a Linux kernel driver for a wireless sensor network. This network contains a number of voltage, temperature and light sensors. Developed a character device driver which takes input from the sensor network and exposes the information to userspace in different device files for each metric. The driver was developed as a Linux kernel module.

Assigment2:
Developed virtual hardware for QEMU-KVM framework. Designed and implemented a virtual VirtIO cryptographic device as part of QEMU. The device allows applications running inside the VM to access the real host crypto device using paravirtualization. The device was implemented using the split-driver model: a frontend (inside the VM) and a backend (part of QEMU). The frontend exposes to user space applications the same API as the host cryptodev while the backend receives calls from the frontend and forwards them for processing by the host cryptodev. To test the driver's functionality, an ecrypted chat application over TCP/IP sockets was implemented.
 
 
 

