Collaboration:
Lanfang Zhang: 	double signal sending/receiving; 
		error handle;
		input/output interleaving; 
		message boundaries
		documentation;

Jiahui Wang: 	convert string to binary array; 
		convert binary array to string 
                single signal sending/receiving; 
                error handle;
		

The code was tested on the physical hosts in lab CSC 219.

1. How do you code messages for the one signal option (only SIGUSR1 used)?
	As a sender, after getting the message, we convert each character to an 8 bits binary array. 
	Then do a for loop to to through each bit:
		If it is ‘1’, then send SIGUSR1 to the target process then call ’usleep(5000)’ to delays program for 5000 us, which give receiver some time to deal the sigal (eg. add '1' to buffer queue))
		If it is ‘0’, send SIGUSR1, then delay 2000us, send another SIGUSR1, delay 3000us. which means we will send two SIGUSR1 whin 5000us. 
		The period for sending one bit is 5000us.
	As a receiver, after received a SIGUSR1:
		if the lastTime = 0 (which means this is the first time the process invokes the handler), we update lastTime to the current time and append ‘1’ to the buffer.(so if the first bits is 0, we can remove this 1 and add a 0 instead without worry the empty queue)
		Otherwise, we compare the time we received this signal with the time we received last signal. If the time difference is less than 2500us (correspond to the 2000us delay in sending), means the send send two SIGUSR1 with 5000us. so the actual bit sender want to send is '0'. we remove previous ‘1’ at the back of buffer queue and append ‘0’ instead. Otherwise, append one ‘1’ at the back of buffer queue and update the 'lastTime' to currentTime.
		

2. How do you code messages for the two signals option (both SIGUSR1 and SIGUSR2 used)?
	As a sender, after getting the message, we convert each character to an 8 bits binary array. 
	Then do a for loop to go through each bit: 
		if it is ‘1’, then send SIGUSR1 to the target process
		if it is ‘0’, send SIGUSR2 to the target process. 
		Between each two sendings, we call ‘usleep(5000)’ to delays program execution for 5000us. The usleep(5000) will give the reciever time to handle the signal.
	As a receiver, once we received an signal, we check it is SIGUSR1 or SIGUSR2. 
		If it is SIGUSR1, we append ‘1’ to the ‘buffer’ queue; 
		otherwise, append ‘0’. 
		When we get 8 bits (8 bits = 1 char), we read those bits and convert them to a character.

3. How do you represent message boundaries?
	The period for sending one bits is 5000us, 8 bits is 40000us. Once finish sending, we call ‘usleep(10000)’  to delays program execution for 100000us (which is way more longer than 40000us), then send one more signal (SIGUSR1) to the target process. 
	In the target process, we check the current system time and the time we received the last signal. Compute the difference, if the difference is bigger than 80000us (still, way more longer than 40000us), we reach the end of the message; otherwise, keep waiting.
	Using time interval as end mark instead of 8 consecutive 1, can avoid the sending error. 

4. How do you check for errors?
	Once finish sending the content of the message, we send eight more signals (SIGUSR1) to the target process. The period for sending one bit is als 5000us.
	In the target process, we check every eight signals, if we do not receive 8 consecutive SIGUSR1, then we know the error happened.
	Also, if we received 8 consecutive singal which starts with 1 (such as 10001000), then we know the error happened. (there is no such character) 

5. What errors may you not be able to catch?
	If the handler miss the end mark, the message will not be print, and it will make the next message error.
	The handler may not find this error if it receives 8 consecutive SIGUSR1 but not at the end of message.
	if the target proccess exit. current process has no idea where the message will send to 

6. How do you handle the interleaving of input/output?
	We set the handler flag to SA_Restart in order to restart the process. 
	Our program checks if the buffer queue and message queue is empty to determine if the program is receiving. 
	if the buffer queue and message queue is empty, means the process is waiting. (those two queue will only be nuke when the receiveing ending)
	if the buffer queue and message queue is not empty, means the process is recieving. (once the process receive a signal, it will add 1 or 0 to buffer)


7. Is there any other signal you had to use (and why)?
	No.

8. How do you ensure that you do not spend unnecessary CPU cycles?
	Since Process do noy consume CPU resources while they are sleeping, which means the CPU will be able to run other programs that have work to do while current process waits.
	


