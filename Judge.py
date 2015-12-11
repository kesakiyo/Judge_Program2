from pymysql import connect, Error, sys, cursors
from time import sleep
from natsort import natsorted, ns
from Crypto.Cipher import AES
import json
import time
import shutil
import shlex
import subprocess
import random
import base64
import os
import re

#Wait 							0
#Judging						1
#Accept							2
#Wrong Answer					3
#Compile Error 					4
#Runtime Error 					5
#Memory Limit Exceeded 			6
#Time Limit Exceeded 			7
#Output Limit Exceeded			8
WAIT, JUDGING, AC, WA, CE, RE, ML, TL, OL = 0, 1, 2, 3, 4, 5, 6, 7, 8
source_name = ["empty", "Main.c", "Main.cc", "Main.cc"]
container_name = ["empty", "baekjoon/onlinejudge-gcc:4.8", "baekjoon/onlinejudge-gcc:4.8", "baekjoon/onlinejudge-gcc:4.8"]
lang = ["empty", "C", "CC", "CC11"]


# the block size for the cipher object; must be 16, 24, or 32 for AES
BLOCK_SIZE = 32

# the character used for padding--with a block cipher such as AES, the value
# you encrypt must be a multiple of BLOCK_SIZE in length.  This character is
# used to ensure that your value is always a multiple of BLOCK_SIZE
PADDING = "{"

# one-liner to sufficiently pad the text to be encrypted
def pad(s): 
    return s + (BLOCK_SIZE - len(s) % BLOCK_SIZE) * PADDING

# encrypt with AES, encode with base64
def EncodeAES(plaindata, key):
    key = pad(key)
    cipher = AES.new(key)
    enc = cipher.encrypt(pad(plaindata))#cipher.encrypt(s)#
    return base64.b64encode(enc)


# Load Mysql Connection information.
with open("./connect_info.json") as f:
	conn_info = json.load(f)

while True:
	try:
		# Connect to the database
		conn = connect(host = conn_info["host"], port = int(conn_info["port"]), user = conn_info["user"], passwd = conn_info["passwd"], db = conn_info["db"])

		#Setting cursors and defining type of returns we need from Database, here it"s gonna be returning as dictionary data
		submissions = conn.cursor(cursors.DictCursor);
		problems = conn.cursor(cursors.DictCursor);
		submissions.execute("SELECT * FROM submissions WHERE result = " + str(WAIT))

		for submission in submissions:
			# get problem information
			problem_id = str(submission["problem_id"])
			problems.execute("SELECT * FROM problems WHERE id = " + problem_id)
			problem = problems.fetchone()

			# get submission information
			submission_id = str(submission["id"])
			language = lang[submission["language_id"]]
			time_limit = str(problem["timelimit"])
			memory_limit = str(problem["memorylimit"])
			source = str(submission["code"])
			special = str(problem["special"])

			# get judge data information
			judge_data_path = os.getcwd() + "/JudgeData/" + problem_id
			judge_data_files = natsorted(os.listdir(judge_data_path))

			# judging result.
			user_result, max_time, max_memory = 0, 0, 0

			subprocess.call(["docker", "rm", "coj"])

			# start judgin
			for x in range(int(len(judge_data_files)/2)):
				# clean JudgeArea folder...
				if os.path.exists("JudgeArea"): shutil.rmtree("JudgeArea")
				if not os.path.exists("JudgeArea"): os.makedirs("JudgeArea")

				# file name encryption
				key = str(random.randint(1000, 9999))
				judge_input = (str(EncodeAES(judge_data_files[x*2], key)) + ".in");
				judge_output = (str(EncodeAES(judge_data_files[x*2+1], key)) + ".out");
				judge_input = re.sub(r"[^a-zA-Z0-9.]", "a", judge_input);
				judge_output = re.sub(r"[^a-zA-Z0-9.]", "a", judge_output);

				# make symbolic link for judge data
				os.symlink(judge_data_path + "/" + judge_data_files[x*2], os.getcwd() + "/JudgeArea/" + judge_input);
				os.symlink(judge_data_path + "/" + judge_data_files[x*2+1], os.getcwd() + "/JudgeArea/" + judge_output)

				with open("JudgeArea/" + source_name[submission["language_id"]], "w+") as f:
					f.write("%s" % source)

				#int problem_id = 0;
				#const char* lang = "CC";
				#int special = 0;
				#int time_limit = 1;
				#int memory_limit = 128;
				#const char* judge_input = "data.in";
				#const char* judge_output = "data.out";
				#const char* user_output = "user.out";

				# docker command.....
				# stream = "docker run -v /home/minho/ProblemSolving:/home/minho/ProblemSolving:ro -v /home/minho/ProblemSolving/c++:/home/minho/ProblemSolving/c++:rw --name test --rm=true baekjoon/onlinejudge-gcc:4.8 sh -c"
				# command = stream.split(' ')
				# command.append('/home/minho/ProblemSolving/c++/ps 10 20')
				stream = "docker run -i -t -w " + os.getcwd() + "/JudgeArea -v " + os.getcwd() + ":" + os.getcwd() + ":ro -v " + os.getcwd() + "/JudgeArea" + ":" + os.getcwd() + "/JudgeArea:rw -v " + judge_data_path + ":" + judge_data_path + ":ro --name coj --rm=true --privileged=true " + container_name[submission["language_id"]] + " sh -c"
				command = stream.split()
				addCommand = os.getcwd() + "/judge" + " " + problem_id + " " + language + " " + special + " " + time_limit + " " + memory_limit + " " + judge_input + " " + judge_output
				command.append(addCommand)

				with open("./JudgeArea/result.json", "w+") as f:
					subprocess.call(command, stdout=f)

				# read result.json file. Maintain max value.
				with open("./JudgeArea/result.json") as result_file:
					result_data = json.load(result_file)
					result = int(result_data["result"])
					if result != AC:
						max_memory = max_time = -1
						print("problem_id(%s), data(%s) : " % (problem_id, judge_data_files[x*2]), end=" => ")
						break
					max_time = max(max_time, int(result_data["maxtime"]))
					max_memory = max(max_memory, int(result_data["maxmemory"]))

					print("problem_id(%s), data(%s) : " % (problem_id, judge_data_files[x*2]), end=" => ")
					print(result_data)

				# clean judge area
				if os.path.exists("JudgeArea"): shutil.rmtree("JudgeArea")

			query = "UPDATE submissions SET result = %s, maxmemory = %s, maxtime = %s WHERE id = %s;" % (result, max_memory, max_time, submission_id)
			print(query)
			try:
				submissions.execute(query)
				conn.commit()
			except Error as error:
				print(error)

	except Error as error:
		print(error)

	finally:
		conn.close()
		submissions.close()
		problems.close()
	sleep(1)