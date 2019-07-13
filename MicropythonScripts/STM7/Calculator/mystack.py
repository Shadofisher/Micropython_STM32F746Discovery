class Stack:
	mystack_ptr=0;
	mystack = [];
	def __init__(self):
		self.mystack_ptr=0;
		self.mystack=[];

	def push(self,data):
		self.mystack.append(data)
		self.mystack_ptr = self.mystack_ptr + 1;
		print(self.mystack_ptr);
	
	def pop(self):
		self.mystack_ptr = self.mystack_ptr-1;
		print(self.mystack_ptr)
		return self.mystack.pop();


	def get(self,index):
		return self.mystack[index];

	def get_index(self):
		return self.mystack_ptr;

