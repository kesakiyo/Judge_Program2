from pymysql import connect, Error, sys, cursors
import json

with open("./connect_info.json") as f:
	conn_info = json.load(f)


try:
	conn = connect(host = conn_info["host"], port = int(conn_info["port"]), user = conn_info["user"], passwd = conn_info["passwd"], db = conn_info["db"])
	submissions = conn.cursor(cursors.DictCursor);

	submissions.execute("SELECT * FROM submissions WHERE (isUpdatable = 0 and result >=2)")
	for submission in submissions:
		problem_id = str(submission["problem_id"])
		user_id = str(submission["user_id"])
		accept = 1 if int(submission["result"]) == 2 else 0

		problems = conn.cursor(cursors.DictCursor);
		solveds = conn.cursor(cursors.DictCursor);

		problems.execute("SELECT * FROM problems WHERE id = " + problem_id)
		problem = problems.fetchone()

		submit = int(problem["submit"]) + 1
		answer = int(problem["answer"]) + accept

		submissions_query = "UPDATE submissions SET isUpdatable = 1;"
		problem_query = "UPDATE problems SET submit = %s, answer = %s;" % (submit, answer)
		solveds_query = "UPDATE solveds SET accept = %s WHERE accept = 0;" % accept

		try:
			problems.execute(problem_query)
			submissions.execute(submissions_query)
			solveds.execute(solveds_query)
			conn.commit()

		except Error as error:
			print(error)

except Error as error:
	print(error)