# -*- rd -*-

= Install to Ubuntu --- How to install Cutter to Ubuntu

== Introduction

This document explains how to install Cutter to Ubuntu
Linux.

== Install

We can use aptitude because Cutter provides Debian packages.

There are packages for Ubuntu Precise Pangolin/Saucy Salamander/Trusty Tahr i386/amd64.
If you need a package for another environment,
you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/cutter-users-en>)).

Here are apt lines for for Precise. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://sourceforge.net/projects/cutter/files/ubuntu/ precise main
  deb-src http://sourceforge.net/projects/cutter/files/ubuntu/ precise main

Here are apt lines for for Saucy. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://sourceforge.net/projects/cutter/files/ubuntu/ saucy main
  deb-src http://sourceforge.net/projects/cutter/files/ubuntu/ saucy main

Here are apt lines for for Trusty. We put them into
/etc/apt/sources.list.d/cutter.list.

/etc/apt/sources.list.d/cutter.list:
  deb http://sourceforge.net/projects/cutter/files/ubuntu/ trusty main
  deb-src http://sourceforge.net/projects/cutter/files/ubuntu/ trusty main

Cutter packages are signed by key of cutter-keyring.
Register the key by installing cutter-keyring package.

  % sudo apt-get update
  % sudo apt-get -y --allow-unauthenticated install cutter-keyring

If you install the keyring package, you can install Cutter by apt-get:

  % sudo apt-get update
  % sudo apt-get -y install cutter-testing-framework

== The next step

Installation is completed. We should try
((<tutorial|TUTORIAL>)) with the installed Cutter.
