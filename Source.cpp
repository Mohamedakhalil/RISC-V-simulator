#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <cstring>
using namespace std;

map<string, int> reg;
map<int, string> program_mem;
map<int, int> mem;

bool cont = true;
int pc = 0;

//vector<string> instruction={"LUI", "AUIPC", "JAL", "JALR", "BEQ", "BNE", "BLT",
//                            "BGE", "BLTU", "BGEU", "LB", "LH", "LW", "LBU",
//                            "LHU", "SB", "SH", "SW", "ADDI", "SLTI", "SLTIU", 
//                            "XORI", "ORI", "ANDI", "SLLI", "SRLI", "SRAI", "ADD", 
//                            "SUB", "SLL", "SLT", "SLTU", "XOR", "SRL", "SRA", "OR", "AND", "EXT"};

//hex function converts from binary to hexadecimal
void hex(string binary)
{
    string block;
    string result;
    while (binary.length() % 4 != 0)
    {
        binary.insert(binary.begin(), '0');
    }
    for (int i = 0; i < binary.length(); i = i + 4)
    {
        block = binary.substr(binary.length() - 4 - i, 4);
        if (block == "0000")
            result.insert(result.begin(), '0');
        else if (block == "0001")
            result.insert(result.begin(), '1');
        else if (block == "0010")
            result.insert(result.begin(), '2');
        else if (block == "0011")
            result.insert(result.begin(), '3');
        else if (block == "0100")
            result.insert(result.begin(), '4');
        else if (block == "0101")
            result.insert(result.begin(), '5');
        else if (block == "0110")
            result.insert(result.begin(), '6');
        else if (block == "0111")
            result.insert(result.begin(), '7');
        else if (block == "1000")
            result.insert(result.begin(), '8');
        else if (block == "1001")
            result.insert(result.begin(), '9');
        else if (block == "1010")
            result.insert(result.begin(), 'A');
        else if (block == "1011")
            result.insert(result.begin(), 'B');
        else if (block == "1100")
            result.insert(result.begin(), 'C');
        else if (block == "1101")
            result.insert(result.begin(), 'D');
        else if (block == "1110")
            result.insert(result.begin(), 'E');
        else if (block == "1111")
            result.insert(result.begin(), 'F');
    }
    cout << result;
}

//bin function converts from denary to binary
string bin(int denary)
{
    string result;
    int power = 0;
    while (pow(2, power) <= denary)
    {
        if (pow(2, power + 1) <= denary)
            power++;
        else
            break;
    }
    int num = denary;
    for (int i = 0; i <= power; i++)
    {
        if (pow(2, power - i) <= num)
        {
            result.push_back('1');
            num = num - pow(2, power - i);
        }
        else
            result.push_back('0');
    }
    return result;
}

void parse(string inst, string& dest, string& src, string& srcORimm)
{
    inst = inst.substr(inst.find(' ') + 1, inst.find('\n'));
    dest = inst.substr(0, inst.find(','));
    inst = inst.substr(inst.find(',') + 1, inst.find('\n'));
    src = inst.substr(0, inst.find(','));
    inst = inst.substr(inst.find(',') + 1, inst.find('\n'));
    srcORimm = inst.substr(inst.find(',') + 1, inst.find('\n'));
}

void parseOff(string inst, string& dest, string& src, string& off)
{
    inst = inst.substr(inst.find(' ') + 1, inst.find('\n'));
    dest = inst.substr(0, inst.find(','));
    inst = inst.substr(inst.find(' ') + 1, inst.find('\n'));
    inst = inst.substr(inst.find(',') + 1, inst.find('\n'));
    off = inst.substr(0, inst.find('('));
    inst = inst.substr(inst.find('(') + 1, inst.find('\n'));
    src = inst.substr(0, inst.find(')'));
}

void LUI(string des, int imm)
{
    // Loading Upper Immediate and storing it in reg[des]
    if (des != "x0")
        reg[des] = imm;
}

void AUIPC(string des, int offset)
{
    //Adding the upper immediate to PC and then storing it in reg[des]
    if (des != "x0")
        reg[des] = pc + offset;
}

void JAL(string des, int offset)
{
    // storing the return address at reg[des]
    if (des != "x0")
        reg[des] = pc + 4; //If no return address is desired, provide the x0 as des (unconditional jump behavior)
    // incrementing PC to jump to the address of the desired instruction
    pc += offset;
}

void JALR(string des, string src, int offset)
{
    // Storing the return address in reg[des]
    if (des != "x0")
        reg[des] = pc + 4; //If no return address is desired, provide the x0 as des (unconditional jump behavior)

    // For jumping a long jump, we set PC to the address in reg[src] + offset
    pc = (reg[src] + offset) ^ (-2);
}

void BEQ(string src1, string src2, int offset)
{
    if (reg[src1] == reg[src2])
        pc += offset;
    else
        pc += 4;
}

void BNE(string src1, string src2, int offset)
{
    if (reg[src1] != reg[src2])
        pc += offset;
    else
        pc += 4;
}

void BLT(string src1, string src2, int offset)
{
    if (reg[src1] < reg[src2])
        pc += offset;
    else
        pc += 4;
}

void BGE(string src1, string src2, int offset)
{
    if (reg[src1] >= reg[src2])
        pc += offset;
    else
        pc += 4;
}

void BLTU(string src1, string src2, int offset)
{
    if (unsigned(reg[src1]) < unsigned(reg[src2]))
        pc += offset;
    else
        pc += 4;
}

void BGEU(string src1, string src2, int offset)
{
    if (unsigned(reg[src1]) >= unsigned(reg[src2]))
        pc += offset;
    else
        pc += 4;
}

void LB(string des, string src, int offset)
{
    if (des != "x0")
        reg[des] = (mem[reg[src] + offset] & ((1u << 8) - 1));
}

void LH(string des, string src, int offset)
{
    if (des != "x0")
        reg[des] = (mem[reg[src] + offset] & ((1u << 16) - 1));
}

void LW(string des, string src, int offset)
{
    if (des != "x0")
        reg[des] = mem[reg[src] + offset];
}

void LBU(string des, string src, int offset)
{
    if (des != "x0")
        reg[des] = unsigned(mem[reg[src] + offset] & ((1u << 8) - 1));
}

void LHU(string des, string src, int offset)
{
    if (des != "x0")
        reg[des] = unsigned(mem[reg[src] + offset] & ((1u << 16) - 1));
}

void SB(string des, string src, int offset)
{
    mem[reg[src] + offset] = unsigned(reg[des] & ((1u << 8) - 1));
}

void SH(string des, string src, int offset)
{
    mem[reg[src] + offset] = unsigned(reg[des] & ((1u << 16) - 1));
}

void SW(string des, string src, int offset)
{
    mem[reg[src] + offset] = unsigned(reg[des]);
}

int ADDI(string source, int num)
{
    return (reg[source] + num);
}

int SLTI(string source, int num)
{
    if (reg[source] < num)
        return 1;
    else
        return 0;
}

int SLTIU(string source, int num)
{
    if (unsigned(reg[source]) < unsigned(num))
        return 1;
    else
        return 0;
}

int XORI(string source, int num)
{
    return(reg[source] ^ num);
}

int ORI(string source, int num)
{
    return(reg[source] | num);
}

int ANDI(string source, int num)
{
    return(reg[source] & num);
}

int SLLI(string source, int num)
{
    return(reg[source] << num);
}

int SRLI(string source, int num)
{
    return(unsigned(reg[source]) >> num);
}

int SRAI(string source, int num)
{
    return(reg[source] >> num);
}

int ADD(string src1, string src2)
{
    return(reg[src1] + reg[src2]);
}

int SUB(string src1, string src2)
{
    return(reg[src1] - reg[src2]);
}

int SLL(string src1, string src2)
{
    return(reg[src1] << reg[src2]);
}

int SLT(string src1, string src2)
{
    if (reg[src1] < reg[src2])
        return 1;
    else
        return 0;
}

int SLTU(string src1, string src2)
{
    if (unsigned(reg[src1]) < unsigned(reg[src2]))
        return 1;
    else
        return 0;
}

int XOR(string src1, string src2)
{
    return(reg[src1] ^ reg[src2]);
}

int SRL(string src1, string src2)
{
    return(unsigned(reg[src1]) >> reg[src2]);
}

int SRA(string src1, string src2)
{
    return(reg[src1] >> reg[src2]);
}

int OR(string src1, string src2)
{
    return(reg[src1] | reg[src2]);
}

int AND(string src1, string src2)
{
    return(reg[src1] & reg[src2]);
}

void EXT()
{
    cont = false;
}

void inst_exec(string inst)
{
    string kw = inst.substr(0, inst.find(' '));
    string des, src, srcORimm, offset;

    if (!(_stricmp(kw.c_str(), "LUI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        LUI(des, stoi(src));
        pc += 4;
        cout << "PC after: " << pc << " ";
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "AUIPC")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        AUIPC(des, stoi(src));
        pc += 4;
        cout << "PC after: " << pc << " ";
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "JAL")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        JAL(des, stoi(src));
        cout << "PC after: " << pc << " ";
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "JALR")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        JALR(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " ";
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "BEQ")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BEQ(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "BNE")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BNE(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "BLT")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BLT(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "BGE")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BGE(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "BLTU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BLTU(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "BGEU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        BGEU(des, src, stoi(srcORimm));
        cout << "PC after: " << pc << " " << endl;
    }

    if (!(_stricmp(kw.c_str(), "LB")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        LB(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "LH")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        LH(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "LW")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        LW(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "LBU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        LBU(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "LHU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        LHU(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SB")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        SB(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Wrote Byte: " << mem[reg[src] + stoi(srcORimm)] << " at Memory Address: " << reg[src] + stoi(srcORimm) << endl;
    }

    if (!(_stricmp(kw.c_str(), "SH")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        SH(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Wrote Half Word: " << mem[reg[src] + stoi(srcORimm)] << " at Memory Address: " << reg[src] + stoi(srcORimm) << endl;
    }

    if (!(_stricmp(kw.c_str(), "SW")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parseOff(inst, des, src, srcORimm);
        SB(des, src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Wrote Word: " << mem[reg[src] + stoi(srcORimm)] << " at Memory Address: " << reg[src] + stoi(srcORimm) << endl;
    }

    if (!(_stricmp(kw.c_str(), "ADDI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = ADDI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLTI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLTI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLTIU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLTIU(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "XORI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = XORI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "ORI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = ORI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "ANDI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = ANDI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLLI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLLI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SRLI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SRLI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SRAI")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SRAI(src, stoi(srcORimm));
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "ADD")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = ADD(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SUB")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SUB(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLL")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLL(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLT")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLT(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SLTU")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SLTU(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "XOR")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = XOR(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SRL")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SRL(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "SRA")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = SRA(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "OR")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = OR(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if (!(_stricmp(kw.c_str(), "AND")))
    {
        cout << "Instruction: " << inst << endl;
        cout << "Status: ";
        cout << "PC before:" << pc << " ";
        parse(inst, des, src, srcORimm);
        if (des != "x0")
            reg[des] = AND(src, srcORimm);
        pc += 4;
        cout << "PC after: " << pc << " " << endl;
        cout << "Modified Register " << des << ": " << reg[des] << endl;
    }

    if ((!(_stricmp(kw.c_str(), "FENCE"))) | (!(_stricmp(kw.c_str(), "EBREAK"))) | (!(_stricmp(kw.c_str(), "ECALL"))))
    {
        EXT();
    }
}

int main()
{
    reg["x0"] = 0;
    ifstream file;
    fstream data;
    string comm;
    int start;
    //var start = pc
    //The format is: the first line has the starting address
    //If there are any initialized data, then it will be written in this format: memory_address value
    //For any initialized registers, it will be in the format: register_name value
    //The memory address is in the form of XXXX, where X is a digit (0-9)
    data.open("Test 3.txt");
    if (!data.is_open())
        cout << "Error in opening data file";
    else
    {
        int address, value;
        getline(data, comm);
        start = stoi(comm);
        pc = start;
        while (!data.eof())
        {
            getline(data, comm);
            address = stoi(comm.substr(0, comm.find(' ')));
            value = stoi(comm.substr(comm.find(' ') + 1, comm.find('\n')));
            mem[address] = value;
        }
    }
    data.close();
    file.open("P3.txt");
    if (!file.is_open())
        cout << "Error";
    else
    {
        while (!file.eof())
        {
            getline(file, comm);
            program_mem[pc] = comm;
            pc += 4;
        }
    }
    pc = start;
    while (cont)
    {
        inst_exec(program_mem[pc]);
    }
    file.close();
    remove("Test 3.txt");
    data.open("Test 3.txt", ios::app);
    data << start << endl;
    int counter = 0;
    for (auto i : mem)
    {
        counter++;
        data << i.first << ' ' << i.second;
        if (counter != mem.size())
            data << endl;
    }
    data.close();
    return 0;
}