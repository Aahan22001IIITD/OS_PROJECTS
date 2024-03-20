def pp(p):
    s=''
    for i in p:
        s+=i
    return s
import sys
def decimal_to_binary1(num):
    num=str(num)
    lis=list()
    s=''
    for j in num:
        if j=='.':
            lis.append(s)
            s='.'
        else:
            s+=j
    lis.append(s)
    left=int(lis[0])
    right=lis[1]
    left = bin(left)[2:]
    s=''
    ll='.'
    lut=list()
    for i in range(5):
        right=float(right)*2
        right=str(right)
        lut=right.split(".")
        rr=lut[0]
        ll+=lut[1]
        s+=rr
        right=ll
        ll='.'
    f=str(left)+'.'+s
    return f

def binary_to_decimal2(a):
    expo=a[8:11]
    deci=a[11:]
    expo=int(expo,2)
    expo=expo-3
    wh='1'
    for j in range(expo):
        wh+=deci[j]
    wh=int(wh,2)
    real_deci=''
    for j in range(expo,len(deci)):
        real_deci+=deci[j]
    deci=''
    number=0
    count=-1
    for j in real_deci:
        j=int(j)
        j=j*(2**count)
        number+=(j)
        count-=1
    number=str(number)[1:]
    e=str(wh)+number
    return e

list_instructions=list()
pc='0000000'
def addition(a,b):
    a=int(a,2)
    b=int(b,2)
    s=bin(a+b)
    return s[2:]
def twos(a):
    b=[]
    for i in a:
        b.append(i)
    for i in range (len(b)):
        if b[i] == '1':
            b[i]='0'
        else:
            b[i]='1'
    c=''
    for i in b:
        c+=i
    return add(c,'1')

def add(a: str, b: str) -> str:
    carry = 0
    result = ""
    i = len(a) - 1
    j = len(b) - 1
    while i >= 0 or j >= 0 or carry:
        sum = carry
        if i >= 0:
            sum += int(a[i])
            i -= 1
        if j >= 0:
            sum += int(b[j])
            j -= 1
        result = str(sum % 2) + result
        carry = sum // 2
    return result.zfill(max(len(a), len(b)))

def substraction(ba,bb):
    if len(ba)>len(bb):
        bb=bb.zfill(len(ba))
    elif len(ba)<len(bb):
        ba=ba.zfill(len(bb))
    twos_bb = twos(bb).zfill(len(ba))
    return add(ba,twos_bb)
register_file=dict()
register_file['000']=[16*['0'],'R0']
register_file['001']=[16*['0'],'R1']
register_file['010']=[16*['0'],'R2']
register_file['011']=[16*['0'],'R3']
register_file['100']=[16*['0'],'R4']
register_file['101']=[16*['0'],'R5']
register_file['110']=[16*['0'],'R6']
register_file['111']=[16*['0'],'FLAGS']
flag_prev=16*['0']
dictionary_of_memory_address=dict()
list_of_labels=list()
op_code=['00000','00001','00010','00011','00100','00101','00110','00111','01000','01001','01010','01011','01100','01101','01110','01111','11100','11101','11111','11010','10000','10001','10010']
f=open("assembly_output.txt",'r')
list_all_lines=list()
for i in f:
    list_all_lines.append(i.strip())
    list_instructions.append(pc)
    pc=add(pc,'1')
total=zip(list_all_lines,list_instructions)
pc='0000000'
flag=0
counter=0
while True:
    i=list_all_lines[counter]
    if i=='':
        continue
    elif i[:5]==op_code[0]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        a=''
        b=''
        for j in range(16):
            a+=(s1[j])
            b+=(s2[j])
        c=addition(a,b)
        # print(c[-3])
        for j in range(16):
            if j<len(c):
                d[15-j]=c[len(c)-1-j]
            else:
                d[15-j]='0'
        register_file[i[7:10][0]]=d
    elif i[:5]==op_code[1]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        c=substraction(a,b)
        a=int(a)
        b=int(b)
        for j in range(16):
            if j<len(c):
                d[15-j]=c[len(c)-1-j]
            else :
                d[15-j]='0'
        if (a<b):
            register_file['111'][0][-4]='1'
        register_file[i[7:10]][0]=d
    elif i[:5]==op_code[2]:
        d=register_file[i[6:9]][0]
        s=i[9:16]
        for j in range(16):
            if j<len(s):
                d[15-j]=s[len(s)-1-j]
            else:
                d[15-j]='0'
    elif i[:5]==op_code[3]:
        for j in range(16):
            register_file[i[10:13]][0][j]=register_file[i[13:16]][0][j]
        register_file['111'][0][-1]='0'
        register_file['111'][0][-2]='0'
        register_file['111'][0][-3]='0'
        register_file['111'][0][-4]='0'
    elif i[:5]==op_code[4]:
        try:
            for j in range(16):
                register_file[i[6:9]][0][j]=dictionary_of_memory_address[i[9:16]][j]
        except:
            print(f"{pc}        ",end='')
            pc=add(pc,'1')
            counter+=1
            print(pp(register_file['000'][0]),end=' ')
            print(pp(register_file['001'][0]),end=' ')
            print(pp(register_file['010'][0]),end=' ')
            print(pp(register_file['011'][0]),end=' ')
            print(pp(register_file['100'][0]),end=' ')
            print(pp(register_file['101'][0]),end=' ')
            print(pp(register_file['110'][0]),end=' ')
            print(pp(register_file['111'][0]))
            continue
    elif i[:5]==op_code[5]:
        dictionary_of_memory_address[i[9:16]]=16*['0']
        for j in range(16):
            dictionary_of_memory_address[i[9:16]][j]=register_file[i[6:9]][0][j]
    elif i[:5]==op_code[6]:
            s1=register_file[i[10:13]][0]
            s2=register_file[i[13:16]][0]
            a=''
            b=''
            for j in range(16):
                a+=s1[j]
                b+=s2[j]
            a=int(a,2)
            b=int(b,2)
            c=a*b
            c=bin(c)
            if (len(c)-2)>16:
                register_file[i[7:10]][0]=['0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0']
                register_file['111'][0][-4]='1'
            else:
                for j in range(16):
                    if (len(c)-1-j)>1:
                        register_file[i[7:10]][0][15-j]=c[len(c)-1-j]
                    else:
                        register_file[i[7:10]][0][15-j]='0'

    elif i[:5]==op_code[7]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        if s2.count('0')==16:
            for j in range(16):
                register_file['000'][0][j]='0'
                register_file['001'][0][j]='0'
            register_file['111'][0][-4]='1'
            continue
        quotient=register_file['000'][0]
        remainder=register_file['001'][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        a=int(a,2)
        b=int(b,2)
        c=a//b
        d=a%b
        c=bin(c)[2:]
        d=bin(d)[2:]
        for j in range(16):
            if len(c)>j:
                quotient[15-j]=c[len(c)-j-1]
            else:
                quotient[15-j]='0'
            if len(d)>j:
                remainder[15-j]=d[len(d)-1-j]
            else:
                remainder[15-j]='0'
    elif i[:5]==op_code[8]:
        s=register_file[i[6:9]][0]
        num=i[9:16]
        num=int(num,2)
        a=''
        for j in range(16):
            a+=s[j]
        a=int(a,2)
        c=a//(2**num)
        c=bin(c)[2:]
        for j in range(16):
            if len(c)>j:
                s[15-j]=c[len(c)-1-j]
            else:
                s[15-j]='0'

    elif i[:5]==op_code[9]:
        s=register_file[i[6:9]][0]
        num=i[9:16]
        num=int(num,2)
        a=''
        for j in range(16):
            a+=s[j]
        a=int(a,2)
        c=a*(2**num)
        c=bin(c)[2:]
        for j in range(16):
            if len(c)>j:
                s[15-j]=c[len(c)-1-j]
            else:
                s[15-j]='0'

    elif i[:5]==op_code[10]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        a=int(a,2)
        b=int(b,2)
        c=bin(a^b)[2:]
        for j in range(16):
            if j<len(c):
                d[15-j]=c[len(c)-1-j]
            else:
                d[15-j]='0'

    elif i[:5]==op_code[11]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        a=int(a,2)
        b=int(b,2)
        c=bin(a|b)[2:]
        for j in range(16):
            if j<len(c):
                d[15-j]=c[len(c)-1-j]
            else:
                d[15-j]='0'

    elif i[:5]==op_code[12]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        a=int(a,2)
        b=int(b,2)
        c=bin(a&b)[2:]
        for j in range(16):
            if j<len(c):
                d[15-j]=c[len(c)-1-j]
            else:
                d[15-j]='0'

    elif i[:5]==op_code[13]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        for j in range(16):
            if s2[j]=='0':
                s1[j]='1'
            else:
                s1[j]='0'

    elif i[:5]==op_code[14]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        a=''
        b=''
        for j in range(16):
            a+=s1[j]
            b+=s2[j]
        a=int(a,2)
        b=int(b,2)
        if (a>b):
            register_file['111'][0][-2]='1'
        elif (a<b):
            register_file['111'][0][-3]='1'
        else:
            register_file['111'][0][-1]='1'
        print(f"{pc}        ",end='')
        print(pp(register_file['000'][0]),end=' ')
        print(pp(register_file['001'][0]),end=' ')
        print(pp(register_file['010'][0]),end=' ')
        print(pp(register_file['011'][0]),end=' ')
        print(pp(register_file['100'][0]),end=' ')
        print(pp(register_file['101'][0]),end=' ')
        print(pp(register_file['110'][0]),end=' ')
        print(pp(register_file['111'][0]))
        counter+=1
        pc=add(pc,'1')
        continue

    elif i[:5]==op_code[15]:
        print(f"{pc}        ",end='')
        pc=i[9:16]
        print(pp(register_file['000'][0]),end=' ')
        print(pp(register_file['001'][0]),end=' ')
        print(pp(register_file['010'][0]),end=' ')
        print(pp(register_file['011'][0]),end=' ')
        print(pp(register_file['100'][0]),end=' ')
        print(pp(register_file['101'][0]),end=' ')
        print(pp(register_file['110'][0]),end=' ')
        print(pp(register_file['111'][0]))
        counter=int(pc,2)
        continue
    elif i[:5]==op_code[16]:
        if register_file['111'][0][-3]=='1':
            print(f"{pc}        ",end='')
            pc=i[9:16]
            
            print(pp(register_file['000'][0]),end=' ')
            print(pp(register_file['001'][0]),end=' ')
            print(pp(register_file['010'][0]),end=' ')
            print(pp(register_file['011'][0]),end=' ')
            print(pp(register_file['100'][0]),end=' ')
            print(pp(register_file['101'][0]),end=' ')
            print(pp(register_file['110'][0]),end=' ')
            print(pp(register_file['111'][0]))
            counter=int(pc,2)
            register_file['111'][0][-3]='0'
        else:
            register_file['111'][0][-1]='0'
            register_file['111'][0][-2]='0'
            register_file['111'][0][-3]='0'
            register_file['111'][0][-4]='0'
            pass

    elif i[:5]==op_code[17]:
        if register_file['111'][0][-2]=='1':
            print(f"{pc}        ",end='')
            pc=i[9:16]
            register_file['111'][0][-2]='0'
            print(pp(register_file['000'][0]),end=' ')
            print(pp(register_file['001'][0]),end=' ')
            print(pp(register_file['010'][0]),end=' ')
            print(pp(register_file['011'][0]),end=' ')
            print(pp(register_file['100'][0]),end=' ')
            print(pp(register_file['101'][0]),end=' ')
            print(pp(register_file['110'][0]),end=' ')
            print(pp(register_file['111'][0]))
            counter=int(pc,2)
            continue
        else:
            register_file['111'][0][-1]='0'
            register_file['111'][0][-2]='0'
            register_file['111'][0][-3]='0'
            register_file['111'][0][-4]='0'
            pass

    elif i[:5]==op_code[18]:
        if register_file['111'][0][-1]=='1':
            print(f"{pc}        ",end='')
            pc=i[9:16]
            register_file['111'][0][-1]='0'
            register_file['111'][0][-2]='0'
            register_file['111'][0][-3]='0'
            register_file['111'][0][-4]='0'
            print(pp(register_file['000'][0]),end=' ')
            print(pp(register_file['001'][0]),end=' ')
            print(pp(register_file['010'][0]),end=' ')
            print(pp(register_file['011'][0]),end=' ')
            print(pp(register_file['100'][0]),end=' ')
            print(pp(register_file['101'][0]),end=' ')
            print(pp(register_file['110'][0]),end=' ')
            print(pp(register_file['111'][0]))
            counter=int(pc,2)
            register_file['111'][0][-1]='0'
            continue
        else:
            register_file['111'][0][-1]='0'
            register_file['111'][0][-2]='0'
            register_file['111'][0][-3]='0'
            register_file['111'][0][-4]='0'
            pass
    elif i[:5]==op_code[19]:
        print(f"{pc}        ",end='')
        register_file['111'][0][-1]='0'
        register_file['111'][0][-2]='0'
        register_file['111'][0][-3]='0'
        register_file['111'][0][-4]='0'
        print(pp(register_file['000'][0]),end=' ')
        print(pp(register_file['001'][0]),end=' ')
        print(pp(register_file['010'][0]),end=' ')
        print(pp(register_file['011'][0]),end=' ')
        print(pp(register_file['100'][0]),end=' ')
        print(pp(register_file['101'][0]),end=' ')
        print(pp(register_file['110'][0]),end=' ')
        print(pp(register_file['111'][0]))
        register_file['111'][0][-1]='0'
        register_file['111'][0][-2]='0'
        register_file['111'][0][-3]='0'
        register_file['111'][0][-4]='0'
        break
    elif i[:5]==op_code[20]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        r=''
        u=''
        for j in range(16):
            r+=s1[j]
            u+=s2[j]
        a=float(binary_to_decimal2(r))
        b=float(binary_to_decimal2(u))
        c=a+b
        q=decimal_to_binary1(str(c))
        num=''
        count_deci=-1
        for j in q:
            if j!='.':
                num+=j
                count_deci+=1
            else:
                break
        b=bin(3+count_deci)[2:]
        st=''
        countu=0
        for j in range(1,6):
            if q[j+countu]!='.':
                st+=q[j+countu]
            else:
                st+=q[j+1]
                countu+=1
        count2=8
        for j in b:
            d[count2]=j
            count2+=1
        for j in st:
            d[count2]=j
            count2+=1
    elif i[:5]==op_code[21]:
        s1=register_file[i[10:13]][0]
        s2=register_file[i[13:16]][0]
        d=register_file[i[7:10]][0]
        r=''
        u=''
        for j in range(16):
            r+=s1[j]
            u+=s2[j]
        a=float(binary_to_decimal2(r))
        b=float(binary_to_decimal2(u))
        if (a<b):
            register_file['111'][0][-4]='1'
            for j in range(16):
                d[j]='0'
            continue
        c=b-a
        a=decimal_to_binary1(c)
        count_deci=-1
        for j in a:
            if j!='.':
                count_deci+=1
            else:
                break
        b=bin(3+count_deci)[2:]
        st=''
        countu=0
        for j in range(1,6):
            if a[i+countu]!='.':
                st+=a[j+countu]
            else:
                st+=a[j+1]
                countu+=1
        count2=8
        for j in b:
            d[count2]=j
            count2+=1
        for j in st:
            d[count2]=j
    elif i[:5]==op_code[22]:
        d=register_file[i[5:8]][0]
        s=i[8:16]
        for j in range(16):
            if j<len(s):
                d[15-j]=str(s[len(s)-1-j])
            else:
                d[15-j]='0'
    print(f"{pc}        ",end='')
    pc=add(pc,'1')
    counter+=1
    print(pp(register_file['000'][0]),end=' ')
    print(pp(register_file['001'][0]),end=' ')
    print(pp(register_file['010'][0]),end=' ')
    print(pp(register_file['011'][0]),end=' ')
    print(pp(register_file['100'][0]),end=' ')
    print(pp(register_file['101'][0]),end=' ')
    print(pp(register_file['110'][0]),end=' ')
    print(pp(register_file['111'][0]))
    register_file['111'][0][-1]='0'
    register_file['111'][0][-2]='0'
    register_file['111'][0][-3]='0'
    register_file['111'][0][-4]='0'
    
c=0
for i in list_all_lines:
    print(pp(i))
    c+=1
for i in dictionary_of_memory_address:
    print(pp(dictionary_of_memory_address[i]))
    c+=1
s='0'* 16
for j in range(c,128):
    print(s)
