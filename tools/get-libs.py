#!/usr/bin/python

import subprocess
import os
import stat
import urllib
import tarfile

lib_dir = "../libs"

cod4x17a_repo = "https://github.com/IceNinjaman/CoD4X17a_testing"
zmq_repo      = "http://download.zeromq.org/zeromq-4.0.5.tar.gz"

def pull_repo(repo_url, target_dir):
	try:
		subprocess.check_output(['which', 'git'])
	except subprocess.CalledProcessError as err:
		print("[\033[91m-\033[0m] git not installed, install git and try run this script again")
		return

	if not os.path.exists(target_dir):
		try:
			os.makedirs(target_dir)
		except OSError as err:
			print(err)
			return

	print("[\033[92m+\033[0m] Cloning repo to " + target_dir)

	try:
		subprocess.check_output(['git', 'clone', repo_url, target_dir])
	except subprocess.CalledProcessError as err:
		return

def pull_tar(tar_url, target_dir):
	print("[\033[92m+\033[0m] Downloading tar")
	tar = urllib.URLopener()
	tar.retrieve(tar_url, target_dir + "/" + tar_url.split('/')[-1])

def untar(tar_path):
	if (tar_path.endswith("tar.gz")):
		tar = tarfile.open(tar_path)
		tar.extractall()
		tar.close()
		print "[\033[92m+\033[0m] Extracted tar"
	else:
		print "Not a tar.gz file: '%s '" % sys.argv[0]


# =======================================================================================================

# Ensure we have the libs folder
if not os.path.exists(lib_dir):
	try:
		os.makedirs(lib_dir)
	except OSError as err:
		print(err)

os.chdir(lib_dir)

# We need the CoD4X17 server stuff, we don't need its deps only the headers for compiling the
# actual plugin
if not os.path.exists(lib_dir + "/CoD4X17a"):
	pull_repo(cod4x17a_repo, lib_dir + "/CoD4X17a")
else:
	print("[\033[92m+\033[0m] CoD4X17a repo already cloned, continueing")

# Pull down ZMQ
pull_tar(zmq_repo, lib_dir)
untar(lib_dir + "/" + zmq_repo.split('/')[-1])

# Start ZMQ compiling
os.chdir("")