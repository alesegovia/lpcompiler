;
; The L+ Standard Library (lp std lib)
;

; >> METHOD 1 <<
.method public <init>()V
    .limit stack 1
    .limit locals 1
.line 20
.var 0 is this Llpstdlib; from Label1 to Label2
Label1:
    aload_0
    invokenonvirtual java/lang/Object/<init>()V
    return
Label2:
.end method

; >> METHOD 2 <<
.method public static toString(I)Ljava/lang/String;
    .limit stack 1
    .limit locals 1
.line 25
.var 0 is i I from Label1 to Label2
Label1:
    iload_0
    invokestatic java/lang/Integer/toString(I)Ljava/lang/String;
    areturn
Label2:
.end method

; >> METHOD 3 <<
.method public static toString(Z)Ljava/lang/String;
    .limit stack 1
    .limit locals 1
.line 30
.var 0 is b Z from Label1 to Label2
Label1:
    iload_0
    invokestatic java/lang/Boolean/toString(Z)Ljava/lang/String;
    areturn
Label2:
.end method

; >> METHOD 4 <<
.method public static toString(F)Ljava/lang/String;
    .limit stack 1
    .limit locals 1
.line 35
.var 0 is f F from Label1 to Label2
Label1:
    fload_0
    invokestatic java/lang/Float/toString(F)Ljava/lang/String;
    areturn
Label2:
.end method

; >> METHOD 5 <<
.method public static toFloat(Ljava/lang/String;)F
    .limit stack 1
    .limit locals 1
.line 40
.var 0 is s Ljava/lang/String; from Label1 to Label2
Label1:
    aload_0
    invokestatic java/lang/Float/parseFloat(Ljava/lang/String;)F
    freturn
Label2:
.end method

; >> METHOD 6 <<
.method public static toInt(Ljava/lang/String;)I
    .limit stack 1
    .limit locals 1
.line 45
.var 0 is s Ljava/lang/String; from Label1 to Label2
Label1:
    aload_0
    invokestatic java/lang/Integer/parseInt(Ljava/lang/String;)I
    ireturn
Label2:
.end method

; >> METHOD 7 <<
.method public static trunc(F)I
    .limit stack 1
    .limit locals 1
.line 50
.var 0 is f F from Label1 to Label2
Label1:
    fload_0
    f2i
    ireturn
Label2:
.end method

; >> METHOD 8 <<
.method public static print(Ljava/lang/String;)V
    .limit stack 2
    .limit locals 1
.line 55
.var 0 is s Ljava/lang/String; from Label1 to Label2
Label1:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    aload_0
    invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
.line 56
    return
Label2:
.end method

; >> METHOD 9 <<
.method public static print(I)V
    .limit stack 2
    .limit locals 1
.line 60
.var 0 is i I from Label1 to Label2
Label1:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    iload_0
    invokevirtual java/io/PrintStream/println(I)V
.line 61
    return
Label2:
.end method

; >> METHOD 10 <<
.method public static print(F)V
    .limit stack 2
    .limit locals 1
.line 65
.var 0 is f F from Label1 to Label2
Label1:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    fload_0
    invokevirtual java/io/PrintStream/println(F)V
.line 66
    return
Label2:
.end method

; >> METHOD 11 <<
.method public static print(Z)V
    .limit stack 2
    .limit locals 1
.line 70
.var 0 is b Z from Label1 to Label2
Label1:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    iload_0
    invokevirtual java/io/PrintStream/println(Z)V
.line 71
    return
Label2:
.end method

; >> METHOD 12 <<
.method public static read()Ljava/lang/String;
    .limit stack 5
    .limit locals 3
.line 75
.var 0 is s Ljava/lang/String; from Label1 to Label7
Label1:
    new java/io/BufferedReader
    dup
    new java/io/InputStreamReader
    dup
    getstatic java/lang/System/in Ljava/io/InputStream;
    invokenonvirtual java/io/InputStreamReader/<init>(Ljava/io/InputStream;)V
    invokenonvirtual java/io/BufferedReader/<init>(Ljava/io/Reader;)V
    astore_1
.line 78
.var 1 is in Ljava/io/BufferedReader; from Label2 to Label7
.catch java/io/IOException from Label2 to Label3 using Label4
Label2:
    aload_1
    invokevirtual java/io/BufferedReader/readLine()Ljava/lang/String;
    areturn
.line 83
Label3:
    goto Label6
.line 80
Label4:
    astore_2
.line 82
.var 2 is ex Ljava/io/IOException; from Label5 to Label6
Label5:
    getstatic java/lang/System/out Ljava/io/PrintStream;
    ldc "I/O Error: unable to read stdin"
    invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
    ldc "I/O Error"
.line 84
Label6:
    areturn
Label7:
.end method

