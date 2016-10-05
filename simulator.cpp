#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

//structure for storing a label and its address
struct LabelTable
{
	string label;
	int32_t address;
};

//structure for storing a memory element's label and value
struct MemoryElement
{
	string label;
	int32_t value;
};

//class for the MIPS Simulator
class MIPSSimulator
{
	private:
		string Registers[32]; //array to store names of registers
		int32_t RegisterValues[32]; //array to store values of registers
		string InstructionSet[17]; //array to store names of instructions
		int32_t Mode; //to store the Mode of execution
		vector<string> InputProgram; //to store the input program
		int32_t NumberOfInstructions; //to store the number of lines in the program
		string current_instruction; //to store the current instruction being worked with
		int32_t ProgramCounter; //to store the line number being worked with
		int32_t MaxLength; //to store the maximum length of the input program
		int32_t halt_value; //flag to check if program halted
		int32_t r[3]; //to store register names, values, etc. for the instruction
		vector<struct LabelTable> TableOfLabels; //to store all the labels and addresses
		vector<struct MemoryElement> Memory; //to store all the memory elements
		int32_t Stack[100]; //stack array
		void add();
		void addi();
		void sub();
		void mul();
		void andf();
		void andi();
		void orf();
		void ori();
		void nor();
		void slt();
		void slti();
		void lw();
		void sw();
		void beq();
		void bne();
		void j();
		void halt();
		void preprocess();
		void ReadInstruction(int32_t line);
		int32_t ParseInstruction();
		void ReportError();
		void ExecuteInstruction(int32_t instruction);
		void OnlySpaces(int32_t lower, int32_t upper, string str);
		void RemoveSpaces(string &str);
		void assertNumber(string str);
		void findRegister(int32_t number);
		string findLabel();
		void assertRemoveComma();
		void checkStackBounds(int32_t index);
		void assertLabelAllowed(string str);

	public:
		MIPSSimulator(int32_t mode, string fileName);
		void Execute();
		void displayState();
};
int32_t sorttable(LabelTable a, LabelTable b);
int32_t sortmemory(MemoryElement a, MemoryElement b);

//function to run the simulator
void MIPSSimulator::Execute()
{
	preprocess(); //populate list of memory elements and labels
	while(ProgramCounter<NumberOfInstructions && halt_value==0) //traverse instructions till end or till halt
	{
		ReadInstruction(ProgramCounter); //read instruction
		RemoveSpaces(current_instruction);
		if(current_instruction=="") //ignore blank instructions
		{
			ProgramCounter++;
			continue;
		}
		int32_t instruction=ParseInstruction(); //get operationID
		ExecuteInstruction(instruction);
		if(instruction<13 || instruction>15) //if not jump, update ProgramCounter here
		{
			ProgramCounter++;
		}
		if(Mode==0) //if step by step mode, display state and wait
		{
			displayState();
			getchar();
		}
	}
	displayState(); //display state at end
	if(halt_value==0) //if program ended without halt
	{
		cout<<"Error: Program ended without halt"<<endl;
		exit(1);
	}
}

//constructor to initialize values for the simulator and pass the mode and the input file path
MIPSSimulator::MIPSSimulator(int32_t mode, string fileName)
{
	MaxLength=10000; //maximum allowed length of input file
	NumberOfInstructions=0;
	ProgramCounter=0;
	halt_value=0;
	Memory.clear();
	TableOfLabels.clear();
	string tempRegisters[]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6",
	"t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"}; //names of registers
	for(int32_t i=0;i<32;i++)
	{
		Registers[i]=tempRegisters[i];
	}
	for(int32_t i=0;i<32;i++)
	{
		RegisterValues[i]=0; //initialize registers to 0
	}
	string tempInstructionSet[]={"add","sub","mul","and","or","nor","slt","addi","andi","ori","slti","lw",
	"sw","beq","bne","j","halt"}; //names of instructions allowed
	for(int32_t i=0;i<17;i++)
	{
		InstructionSet[i]=tempInstructionSet[i];
	}
	for(int32_t i=0;i<100;i++)
	{
		Stack[i]=0; //initialize stack elements to 0
	}
	RegisterValues[29]=40396; //stack pointer at bottom element
	Mode=mode; //set mode
	ifstream InputFile;
	InputFile.open(fileName.c_str(),ios::in); //open file
	if(!InputFile) //if open failed
	{
		cout<<"Error: File does not exist or could not be opened"<<endl;
		exit(1);
	}
	string tempString;
	while(getline(InputFile,tempString)) //read line by line
	{
		NumberOfInstructions++;
		if(NumberOfInstructions>MaxLength) ///check number of instructions with maximum allowed
		{
			cout<<"Error: Number of lines in input too large, maximum allowed is "<<MaxLength<<" line"<<endl;
			exit(1);
		}
		InputProgram.push_back(tempString); //store in InputProgram
	}
	InputFile.close();
}

//function to store label names and addresses and memory names and values from the whole program
void MIPSSimulator::preprocess()
{
	int32_t i=0,j=0;
	int32_t current_section=-1; //current_section=0 - data section, current_section=1 - text section
	int32_t index; //to hold index of ".data"
	int32_t commentindex;
	int32_t flag=0; //whether "..data" found
	string tempString="";
	int	isLabel=0;
	int	doneFlag=0;
	int32_t dataStart=0; //line number for start of data section
	int32_t textStart=0;
	for(i=0;i<NumberOfInstructions;i++)
	{
		ReadInstruction(i);
		if(current_instruction=="")
		{
			continue;
		}
		index=current_instruction.find(".data"); //search for beginning of data section
		if(index==-1) //not found
		{
			continue;
		}
		else if(flag==0)
		{
			flag=1; //set as found
			OnlySpaces(0,index,current_instruction); //assert that nothing is before
			OnlySpaces(index+5,current_instruction.size(),current_instruction); //assert that nothing is after
			current_section=0; //set section
			dataStart=i; //set location where section starts
		}
		else if(flag==1) //for multiple findings of ".data"
		{
			cout<<"Error: Multiple instances of .data"<<endl;
			ReportError();
		}
	}
	int32_t LabelIndex; //to store index of ":"
	if(current_section==0) //in data section
	{
		for(i=dataStart+1;i<NumberOfInstructions;i++)
		{
			ReadInstruction(i);
			RemoveSpaces(current_instruction);
			if(current_instruction=="")
			{
				continue;
			}
			LabelIndex=current_instruction.find(":");
			if(LabelIndex==-1) //if ":" not found
			{
				if(current_instruction.find(".text")==-1) //if text section has not started
				{
					cout<<"Error: Unexpected symbol in data section"<<endl;
					ReportError();
				}
				else
				{
					break;
				}
			}
			if(LabelIndex==0) //if found at first place
			{
				cout<<"Error: Label name expected"<<endl;
				ReportError();
			}
			j=LabelIndex-1; //ignore spaces before ":" till label
			while(j>=0 && current_instruction[j]==' ' || current_instruction[j]=='\t')
			{
				j--;
			}
			tempString=""; //to store label name
	//		int32_t isLabel=0; //label found
			int32_t doneFlag=0; //label name complete
			for(;j>=0;j--)
			{
				if(current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFlag==0) //till label name characters are being found
				{
		//			isLabel=1;
					tempString=current_instruction[j]+tempString;
				}
				else if(current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFlag==1) //if something found after name ends
				{
					cout<<"Error: Unexpected text before label name"<<endl;
					ReportError();
				}
	/*			else if(isLabel==0) //i
				{
					cout<<"Error: Label name expected"<<endl;
					ReportError();
				}*/
				else //name ended
				{
					doneFlag=1;
				}
			}
			assertLabelAllowed(tempString); //check validity of name
			MemoryElement tempMemory;
			tempMemory.label=tempString; //create memory and store memory element
			int32_t wordIndex=current_instruction.find(".word"); //search for ".word" in the same way
			if(wordIndex==-1)
			{
				cout<<"Error: .word not found"<<endl;
				ReportError();
			}
			OnlySpaces(LabelIndex+1,wordIndex,current_instruction);
			int32_t foundValue=0;
			int32_t doneFinding=0;
			tempString="";
			for(j=wordIndex+5;j<current_instruction.size();j++)
			{
				if(foundValue==1 && (current_instruction[j]==' ' || current_instruction[j]=='\t') && doneFinding==0)
				{
					doneFinding=1;
				}
				else if(foundValue==1 && current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFinding==1)
				{
					cout<<"Error: Unexpected text after value"<<endl;
					ReportError();
				}
				else if(foundValue==0 && current_instruction[j]!=' ' && current_instruction[j]!='\t')
				{
					foundValue=1;
					tempString=tempString+current_instruction[j];
				}
				else if(foundValue==1 && current_instruction[j]!=' ' && current_instruction[j]!='\t')
				{
					tempString=tempString+current_instruction[j];
				}
			}
			assertNumber(tempString); //check that number found is a valid integer
			tempMemory.value=stoi(tempString); //change type and store
	 		Memory.push_back(tempMemory); //add to list
		}
	}
	sort(Memory.begin(),Memory.end(),sortmemory); //sort for easy comparison
	for(i=0;Memory.size()>0 && i<Memory.size()-1;i++) //check for duplicates
	{
		if(Memory[i].label==Memory[i+1].label)
		{
			cout<<"Error: One or more labels are repeated"<<endl;
			exit(1);
		}
	}
	int32_t textFlag=0;
	int32_t textIndex=0;
	for(i=ProgramCounter;i<NumberOfInstructions;i++)
	{
		ReadInstruction(i);
		if(current_instruction=="")
		{
			continue;
		}
		textIndex=current_instruction.find(".text"); //find text section similar as above
		if(textIndex==-1)
		{
			continue;
		}
		else if(textFlag==0)
		{
			textFlag=1;
			OnlySpaces(0,textIndex,current_instruction);
			OnlySpaces(textIndex+5,current_instruction.size(),current_instruction);
			current_section=1;
			textStart=i;
		}
		else if(textFlag==1)
		{
			cout<<"Error: Multiple instances of .text"<<endl;
			ReportError();
		}
	}
	if(current_section!=1) //if text section not found
	{
		cout<<"Error: Text section does not exist or found unknown string"<<endl;
		exit(1);
	}
	int32_t MainIndex=0; //location of main label
	int32_t foundMain=0; //whether main label found
	LabelIndex=0;
	for(i=textStart+1;i<NumberOfInstructions;i++)
	{
		ReadInstruction(i);
		if(current_instruction=="")
		{
			continue;
		}
		LabelIndex=current_instruction.find(":"); //find labels similar as above
		if(LabelIndex==0)
		{
			cout<<"Error: Label name expected"<<endl;
			ReportError();
		}
		if(LabelIndex==-1)
		{
			continue;
		}
		j=LabelIndex-1;
		while(j>=0 && current_instruction[j]==' ' || current_instruction[j]=='\t')
		{
			j--;
		}
		tempString="";
		isLabel=0;
		doneFlag=0;
		for(;j>=0;j--)
		{
			if(current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFlag==0)
			{
				isLabel=1;
				tempString=current_instruction[j]+tempString;
			}
			else if(current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFlag==1)
			{
				cout<<"Error: Unexpected text before label name"<<endl;
				ReportError();
			}
			else if(isLabel==0)
			{
				cout<<"Error: Label name expected"<<endl;
				ReportError();
			}
			else
			{
				doneFlag=1;
			}
		}
		assertLabelAllowed(tempString);
		OnlySpaces(LabelIndex+1,current_instruction.size(),current_instruction); //check that nothing is after label
		if(tempString=="main") //for main, set variables as needed
		{
			foundMain=1;
			MainIndex=ProgramCounter+1;
		}
		else
		{
			LabelTable tempLabel;
			tempLabel.address=ProgramCounter;
			tempLabel.label=tempString;
			TableOfLabels.push_back(tempLabel); //store labels
		}
	}
	sort(TableOfLabels.begin(),TableOfLabels.end(),sorttable); //sort labels
	for(i=0;TableOfLabels.size()>0 && i<(TableOfLabels.size()-1);i++) //check for duplicates
	{
		if(TableOfLabels[i].label==TableOfLabels[i+1].label)
		{
			cout<<"Error: One or more labels are repeated"<<endl;
			exit(1);
		}
	}
	if(foundMain==0) //if main label not found
	{
		cout<<"Error: Could not find main"<<endl;
		exit(1);
	}
	ProgramCounter=MainIndex; //set ProgramCounter
	cout<<"Initialized and ready to execute. Current state is as follows:"<<endl;
	displayState();
	cout<<endl<<"Starting execution"<<endl<<endl;
}

//function to display line number and instruction at which error occurred and exit the program
void MIPSSimulator::ReportError()
{
	cout<<"Error found in instruction at address: "<<(4*ProgramCounter)<<": "<<InputProgram[ProgramCounter]<<endl;
	displayState();
	exit(1);
}

//function to read an instruction, crop out comments and set the ProgramCounter value
void MIPSSimulator::ReadInstruction(int32_t line)
{
	current_instruction=InputProgram[line]; //set current_instruction
	if(current_instruction.find("#")!=-1) //remove comments
	{
		current_instruction=current_instruction.substr(0,current_instruction.find("#"));
	}
	ProgramCounter=line; //set ProgramCounter
}

//function to find out which instruction is to be executed and and populate values in r[]
int32_t MIPSSimulator::ParseInstruction()
{
	int32_t i=0,j=0;
	RemoveSpaces(current_instruction); //remove spaces
	if(current_instruction.find(":")!=-1) //if label encountered
	{
		return -2;
	}
	if(current_instruction.size()<4) //no valid instruction is this small
	{
		cout<<"Error: Unknown operation"<<endl;
		ReportError();
	}
	for(j=0;j<4;j++) //find length of operation
	{
		if(current_instruction[j]==' ' || current_instruction[j]=='\t')
		{
			break;
		}
	}
	string operation=current_instruction.substr(0,j); //cut the operation out
	if(current_instruction.size()>0 && j<current_instruction.size()-1)
	{
		current_instruction=current_instruction.substr(j+1);
	}
	int32_t foundOp=0; //whether valid operation or not
	int32_t OperationID=-1; //set operation index from array
	for(i=0;i<17;i++) //check operation with allowed operations
	{
		if(operation==InstructionSet[i])
		{
			OperationID=i;
			break;
		}
	}
	if(OperationID==-1) //if not valid
	{
		cout<<"Error: Unknown operation"<<endl;
		ReportError();
	}
	if(OperationID<7) //for R-format instructions
	{
		for(int32_t count=0;count<3;count++) //find three registers separated by commas and put them in r[]
		{
			RemoveSpaces(current_instruction);
			findRegister(count);
			RemoveSpaces(current_instruction);
			if(count==2)
			{
				break;
			}
			assertRemoveComma();
		}
		if(current_instruction!="") //if something more found
		{
			cout<<"Error: Extra arguments provided"<<endl;
			ReportError();
		}
	}
	else if(OperationID<11) //for I-format instructions
	{
		for(int32_t count=0;count<2;count++) //find two registers separated by commas
		{
			RemoveSpaces(current_instruction);
			findRegister(count);
			RemoveSpaces(current_instruction);
			assertRemoveComma();
		}
		RemoveSpaces(current_instruction);
		string tempString=findLabel(); //find third argument, a number
		assertNumber(tempString); //check validity
		r[2]=stoi(tempString); //convert and store
	}
	else if(OperationID<13) //for lw, sw
	{
		string tempString="";
		int32_t offset;
		RemoveSpaces(current_instruction);
		findRegister(0); //find source/destination register
		RemoveSpaces(current_instruction);
		assertRemoveComma(); //find comma, ignoring extra spaces
		RemoveSpaces(current_instruction);
		if((current_instruction[0]>47 && current_instruction[0]<58) || current_instruction[0]=='-') //if offset type
		{
			j=0;
			while(j<current_instruction.size() && current_instruction[j]!=' ' && current_instruction[j]!='\t' && current_instruction[j]!='(')
			{
				tempString=tempString+current_instruction[j]; //find offset
				j++;
			}
			if(j==current_instruction.size()) //if instruction ends there
			{
				cout<<"Error: '(' expected"<<endl;
				ReportError();
			}
			assertNumber(tempString); //check validity of offset
			offset=stoi(tempString); //convert and store
			current_instruction=current_instruction.substr(j);
			RemoveSpaces(current_instruction);
			if(current_instruction=="" || current_instruction[0]!='(' || current_instruction.size()<2)
			{
				cout<<"Error: '(' expected"<<endl;
				ReportError();
			}
			current_instruction=current_instruction.substr(1);
			RemoveSpaces(current_instruction);
			findRegister(1); //find register containing address
			RemoveSpaces(current_instruction);
			if(current_instruction=="" || current_instruction[0]!=')')
			{
				cout<<"Error: ')' expected"<<endl;
				ReportError();
			}
			current_instruction=current_instruction.substr(1);
			OnlySpaces(0,current_instruction.size(),current_instruction);
			r[2]=offset;
			if(r[2]==-1) //-1 reserved for non offset type, anyway an invalid offset, others checked later
			{
				cout<<"Error: Invalid offset"<<endl;
				ReportError();
			}
		}
		else //if label type
		{
			tempString=findLabel(); //find label
			int32_t foundLocation=0;
			for(j=0;j<Memory.size();j++)
			{
				if(tempString==Memory[j].label) //check for label from memory
				{
					foundLocation=1; //label found
					if(OperationID==11)
					{
						r[1]=Memory[j].value; //for lw, send value
					}
					else
					{
						r[1]=j; ///for sw, send index in memory
					}
					break;
				}
			}
			if(foundLocation==0) //if label not found
			{
				cout<<"Error: Invalid label"<<endl;
				ReportError();
			}
			r[2]=-1; //to indicate that it is not offset type
		}
	}
	else if(OperationID<15) //for beq, bne
	{
		for(int32_t count=0;count<2;count++) //find two registers separated by commas
		{
			RemoveSpaces(current_instruction);
			findRegister(count);
			RemoveSpaces(current_instruction);
			assertRemoveComma();
		}
		RemoveSpaces(current_instruction);
		string tempString=findLabel(); //find label
		int32_t foundAddress=0;
		for(j=0;j<TableOfLabels.size();j++) //search for label
		{
			if(tempString==TableOfLabels[j].label)
			{
				foundAddress=1; //if label found
				r[2]=TableOfLabels[j].address; //set r[2]
				break;
			}
		}
		if(foundAddress==0) //if label not found
		{
			cout<<"Error: Invalid label"<<endl;
			ReportError();
		}
	}
	else if(OperationID==15) //for j
	{
		RemoveSpaces(current_instruction);
		int32_t foundAddress=0;
		string tempString=findLabel(); //find jump label
		for(j=0;j<TableOfLabels.size();j++) //search for label
		{
			if(tempString==TableOfLabels[j].label)
			{
				foundAddress=1; //if label found
				r[0]=TableOfLabels[j].address; //set r[0]
			}
		}
		if(foundAddress==0) //if label not found
		{
			cout<<"Error: Invalid label"<<endl;
			ReportError();
		}
	}
	else if(OperationID==16) //for halt
	{
		RemoveSpaces(current_instruction);
	}
	return OperationID;
}

//function to check that between lower and upper-1 indices, str has only spaces, else report an error
void MIPSSimulator::OnlySpaces(int32_t lower, int32_t upper, string str)
{
	for(int32_t i=lower;i<upper;i++)
	{
		if(str[i]!=' ' && str[i]!='\t') //check that only ' ' and '\t' characters exist
		{
			cout<<"Error: Unexpected character"<<endl;
			ReportError();
		}
	}
}

//function to call the appropriate operation function based on the operation to execute
void MIPSSimulator::ExecuteInstruction(int32_t instruction)
{
	switch(instruction) //call appropriate function based on the value of instruction
	{
		case 0:
			add();
			break;
		case 1:
			sub();
			break;
		case 2:
			mul();
			break;
		case 3:
			andf();
			break;
		case 4:
			orf();
			break;
		case 5:
			nor();
			break;
		case 6:
			slt();
			break;
		case 7:
			addi();
			break;
		case 8:
			andi();
			break;
		case 9:
			ori();
			break;
		case 10:
			slti();
			break;
		case 11:
			lw();
			break;
		case 12:
			sw();
			break;
		case 13:
			beq();
			break;
		case 14:
			bne();
			break;
		case 15:
			j();
			break;
		case 16:
			halt();
			break;
		case -2: //if instruction containing label, ignore
			break;
		default:
			cout<<"Error: Invalid instruction received"<<endl;
			ReportError();
	}
}

//function to remove spaces starting from first elements till they exist continuously in str
void MIPSSimulator::RemoveSpaces(string &str)
{
	int32_t j=0;
	while(j<str.size() && (str[j]==' ' || str[j]=='\t')) //till only ' ' or '\t' found
	{
		j++;
	}
	str=str.substr(j); //remove all of those
}

//function to execute add
void MIPSSimulator::add()
{
	if(r[0]==29) //check that value of stack pointer is within bounds
	{
		checkStackBounds(RegisterValues[r[1]]+RegisterValues[r[2]]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1) //cannot modify $zero or use $at
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]+RegisterValues[r[2]]; //execute
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute addi
void MIPSSimulator::addi()
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]+r[2]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]+r[2];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute sub
void MIPSSimulator::sub()//Overflow check to be done?
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]-RegisterValues[r[2]]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]-RegisterValues[r[2]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute mul
void MIPSSimulator::mul()//last 32 bits?
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]*RegisterValues[r[2]]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]*RegisterValues[r[2]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute andf
void MIPSSimulator::andf()
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]&RegisterValues[r[2]]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]&RegisterValues[r[2]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute andi
void MIPSSimulator::andi()
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]&r[2]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]&r[2];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute orf
void MIPSSimulator::orf()
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]|RegisterValues[r[2]]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]|RegisterValues[r[2]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute ori
void MIPSSimulator::ori()
{
	if(r[0]==29)
	{
		checkStackBounds(RegisterValues[r[1]]|r[2]);
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]|r[2];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute nor
void MIPSSimulator::nor()
{
	if(r[0]==29)
	{
		checkStackBounds(~(RegisterValues[r[1]]|RegisterValues[r[2]]));
	}
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=~(RegisterValues[r[1]]|RegisterValues[r[2]]);
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute slt
void MIPSSimulator::slt()
{
	if(r[0]!=0 && r[0]!=1 && r[1]!=1 && r[2]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]<RegisterValues[r[2]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute slti
void MIPSSimulator::slti()
{
	if(r[0]!=0 && r[0]!=1 && r[1]!=1)
	{
		RegisterValues[r[0]]=RegisterValues[r[1]]<r[2];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute lw
void MIPSSimulator::lw()
{
	if(r[0]==29)
	{
		checkStackBounds(r[1]);
	}
	if(r[0]!=0 && r[0]!=1 && r[2]==-1) //if label type
	{
		RegisterValues[r[0]]=r[1];
	}
	else if(r[0]!=0 && r[0]!=1) //if offset type
	{
		checkStackBounds(RegisterValues[r[1]]+r[2]); //check validity of offset
		RegisterValues[r[0]]=Stack[(RegisterValues[r[1]]+r[2]-40000)/4];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute sw
void MIPSSimulator::sw()
{
	if(r[0]!=1 && r[2]==-1) //if label type
	{
		Memory[r[1]].value=RegisterValues[r[0]];
	}
	else if(r[0]!=1) //if offset type
	{
		checkStackBounds(RegisterValues[r[1]]+r[2]); //check validity of offset
		Stack[(RegisterValues[r[1]]+r[2]-40000)/4]=RegisterValues[r[0]];
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute beq
void MIPSSimulator::beq()
{
	if(r[0]!=1 && r[1]!=1)
	{
		if(RegisterValues[r[0]]==RegisterValues[r[1]])
		{
			ProgramCounter=r[2]; //if branch taken, update ProgramCounter with new address
		}
		else
		{
			ProgramCounter++; //else increment as usual
		}
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute bne
void MIPSSimulator::bne()
{
	if(r[0]!=1 && r[1]!=1)
	{
		if(RegisterValues[r[0]]!=RegisterValues[r[1]])
		{
			ProgramCounter=r[2];
		}
		else
		{
			ProgramCounter++;
		}
	}
	else
	{
		cout<<"Error: Invalid usage of registers"<<endl;
		ReportError();
	}
}

//function to execute j
void MIPSSimulator::j()
{
	ProgramCounter=r[0]; //update ProgramCounter to address
}

//function to execute halt
void MIPSSimulator::halt()
{
	halt_value=1; //set halt value to halt the program
}

//function to display the state of the registers and memory
void MIPSSimulator::displayState()
{
	int32_t current_address=40000; //starting address of memory
	if(ProgramCounter<NumberOfInstructions) //display current instruction
	{
		cout<<endl<<"Executing instruction: "<<InputProgram[ProgramCounter]<<endl;
	}
	else
	{
		cout<<endl<<"Executing instruction: "<<InputProgram[ProgramCounter-1]<<endl; //to display at the end, where ProgramCounter==NumberOfInstructions and is out of bounds
	}
	cout<<endl<<"Program Counter: "<<(4*ProgramCounter)<<endl<<endl; //display ProgramCounter
	cout<<"Registers:"<<endl<<endl;
	printf("%11s%12s\t\t%10s%12s\n","Register","Value","Register","Value");
	for(int32_t i=0;i<16;i++) //display registers
	{
		printf("%6s[%2d]:%12d\t\t%5s[%2d]:%12d\n",Registers[i].c_str(),i,RegisterValues[i],Registers[i+16].c_str(),i+16,RegisterValues[i+16]);
	}
	cout<<endl<<"Memory:"<<endl<<endl; //display memory
	printf("%11s%11s%8s\n","Address","Label","Value");
	for(int32_t i=0;i<100;i++) //stack
	{
		printf("%11d%10s:%8d\n",current_address+4*i,"<Stack>",Stack[i]);
	}
	current_address+=400;
	for(int32_t i=0;i<Memory.size();i++) //labels
	{
		printf("%11d%10s:%8d\n",40400+4*i,Memory[i].label.c_str(),Memory[i].value);
	}
	cout<<endl;
}

//function to check that an stoi() on str would be valid, i.e., that str can be converted to an integer
void MIPSSimulator::assertNumber(string str)
{
	for(int32_t j=0;j<str.size();j++) //check that each character is a digit
	{
		if(j==0 && str[j]=='-') //ignore minus sign
		{
			continue;
		}
		if(str[j]<48 || str[j]>57)
		{
			cout<<"Error: Specified value is not a number"<<endl;
			ReportError();
		}
	}
	if(str[0]!='-' && (str.size()>10 || (str.size()==10 && str>"2147483647"))) //check against maximum allowed for 32 bit integer
	{
		cout<<"Error: Number out of range"<<endl;
		ReportError();
	}
	else if(str[0]=='-' && (str.size()>11 || (str.size()==11 && str>"-2147483648"))) //same check as above for negative integers
	{
		cout<<"Error: Number out of range"<<endl;
		ReportError();
	}
}

//function to find which register has been specified and the populate the value in r[number]
void MIPSSimulator::findRegister(int32_t number)
{
	int32_t foundRegister=0;
	if(current_instruction[0]!='$' || current_instruction.size()<2) //find '$' sign
	{
		cout<<"Error: Register expected"<<endl;
		ReportError();
	}
	current_instruction=current_instruction.substr(1); //remove '$' sign
	string registerID=current_instruction.substr(0,2); //next two characters to match
	if(registerID=="ze" && current_instruction.size()>=4) //for $zero, need four characters
	{
		registerID+=current_instruction.substr(2,2);
	}
	else if(registerID=="ze")
	{
		cout<<"Error: Register expected"<<endl;
		ReportError();
	}
	for(int32_t i=0;i<32;i++)
	{
		if(registerID==Registers[i]) //find register from list
		{
			r[number]=i; //populate r[number]
			foundRegister=1; //set flag as found
			if(i!=0) //remove name based on whether $zero or something else
				current_instruction=current_instruction.substr(2);
			else
				current_instruction=current_instruction.substr(4);
		}
	}
	if(foundRegister==0) //if register not found
	{
		cout<<"Error: Invalid register"<<endl;
		ReportError();
	}
}

//function to find and return the label name
string MIPSSimulator::findLabel()
{
	RemoveSpaces(current_instruction); //remove spaces
	string tempString=""; //to store label
	int32_t foundValue=0; //found
	int32_t doneFinding=0; //completed finding
	for(int32_t j=0;j<current_instruction.size();j++)
	{
		if(foundValue==1 && (current_instruction[j]==' ' || current_instruction[j]=='\t') && doneFinding==0)
		{
			doneFinding=1; //when space encountered after start, label finding done
		}
		else if(foundValue==1 && current_instruction[j]!=' ' && current_instruction[j]!='\t' && doneFinding==1)
		{
			cout<<"Error: Unexpected text after value"<<endl; //if non space encountered after done, some incorrect character found
			ReportError();
		}
		else if(foundValue==0 && current_instruction[j]!=' ' && current_instruction[j]!='\t')
		{
			foundValue=1; //when first non space found, finding starts
			tempString=tempString+current_instruction[j];
		}
		else if(foundValue==1 && current_instruction[j]!=' ' && current_instruction[j]!='\t')
		{
			tempString=tempString+current_instruction[j]; //continue finding
		}
	}
	return tempString; //return found label
}

//function to check that first element is a ',' and to remove it
void MIPSSimulator::assertRemoveComma()
{
	if(current_instruction.size()<2 || current_instruction[0]!=',') //check that first element exists and is a comma
	{
		cout<<"Error: Comma expected"<<endl;
		ReportError();
	}
	current_instruction=current_instruction.substr(1); //remove it
}

//function to check that the offset for stack is valid and within bounds
void MIPSSimulator::checkStackBounds(int32_t index)
{
	if(!(index<=40396 && index>=40000 && index%4==0)) //check that address is within stack bounds and a multiple of 4
	{
		cout<<"Error: Invalid address for stack pointer. To access data section, use labels instead of addresses"<<endl;
		ReportError();
	}
}

//function to check that the label name does not start with a number and does not contain special characters
void MIPSSimulator::assertLabelAllowed(string str)
{
	if(str.size()==0 || (str[0]>47 && str[0]<58)) //check that laabel size is at least one and the first value is not an integer
	{
		cout<<"Error: Invalid label"<<endl;
		ReportError();
	}
	for(int32_t i=0;i<str.size();i++)
	{
		if(!((str[i]>47 && str[i]<58) || (str[i]>=65 && str[i]<=90) || (str[i]>=97 && str[i]<=122))) //check that only numbers and letters are used
		{
			cout<<"Error: Invalid label"<<endl;
			ReportError();
		}
	}
}

//function to get ordering for memory elements for sorting
int32_t sortmemory(MemoryElement a, MemoryElement b)
{
	return a.label<b.label; //sort by label
}

//function to get ordering of labels for sorting
int32_t sorttable(LabelTable a, LabelTable b)
{
	return a.label<b.label; //sort by label
}

int main()
{
	string path;
	int32_t mode;
	cout<<"Program to simulate execution in MIPS Assembly language. Two modes are available:"<<endl<<"1. Step by Step Mode"<<endl<<"2. Execution Mode"<<endl<<endl;
	cout<<"Enter the relative path of the input file and the mode:"<<endl;
	cin>>path>>mode;
	if(mode!=1 && mode!=2) //if mode is invalid
	{
		cout<<"Error: Invalid Mode"<<endl;
		return 1;
	}
	MIPSSimulator simulator(mode-1,path); //create and initialize simulator
	simulator.Execute(); //execute simulator
	return 0;
}
