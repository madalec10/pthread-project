# pthread-project
This project aims to use pthreads and mutual exclusion to make a program that performs parallel processing. I made the program in Microsoft Visual Studio Code and I ran it in the Ubuntu Linux virtual machine environment. The input file is an example of the input format this program reads.

The format for the input is [Student Name] [Arrival Time] [Help Time].

There is a Teaching Assistant (TA) office that students are going to for help. There are 2 TAs and 2 chairs outside. If both TAs are occupied when a student arrives then the student will wait in one of the chairs until a TA is available. If both chairs are full and the TAs are occupied then the student will leave without getting help. The program outputs into the console each event that occurs. Additionally, it logs how many students got help, had to wait, or left without help and outputs that information into the console at the end.
