const unsigned char font3x5[] = //like 7-segment display
{
3,5,' ',95,0,
	0x01,0x02,0x03,                    //������
	0x00,0x1D,0x00,                    //!
	 0,0,0,                    //"
    0x1F,0x1F,0x1F,                    //#
	 0,0,0,                    //$
	 0,0,0,                    //%
	 0,0,0,                    //&
	0x00,0x0C,0x00,                    //'
	0x0E,0x11,0x00,                    //(
	0x00,0x11,0x0E,                    //)
	 0,0,0,                    //*
	0x04,0x0E,0x04,                    //+
	0x00,0x03,0x00,                    //,
	0x04,0x04,0x04,                    //-
	0x00,0x01,0x00,                    //.
	 0,0,0,                    ///
	0x1F,0x11,0x1F,                    //0
	0x08,0x1F,0x00,                    //1
	0x09,0x13,0x0D,                    //2
	0x11,0x15,0x0A,                    //3
	0x18,0x04,0x1F,                    //4
	0x1D,0x15,0x12,                    //5
	0x0F,0x15,0x17,                    //6
	0x10,0x13,0x1C,                    //7
	0x1F,0x15,0x1F,                    //8
	0x1D,0x15,0x1E,                    //9
	0x00,0x0A,0x00,                    //:
	0x00,0x0B,0x00,                    //;
	0x04,0x0A,0x00,                    //<
	0x0A,0x0A,0x0A,                    //=
	0x00,0x0A,0x04,                    //>
	 0,0,0,                    //?
	 0,0,0,                    //@
	0x0F,0x14,0x0F,                    //A
	0x1F,0x15,0x0A,                    //B
	0x0E,0x11,0x0A,                    //C
	0x1F,0x11,0x0E,                    //D
	0x1F,0x15,0x15,                    //E
	0x1F,0x14,0x14,                    //F
	0x1D,0x15,0x1E,                    //G=g
	0x1F,0x04,0x1F,                    //H
	0x11,0x1F,0x11,                    //I
	0x11,0x1E,0x10,                    //J
	0x1F,0x04,0x1B,                    //K
	0x1F,0x01,0x01,                    //L
	0x1F,0x08,0x1F,                    //M
	0x0F,0x04,0x07,                    //N=n
	0x0E,0x11,0x0E,                    //O
	0x1F,0x14,0x1C,                    //P
	0x0C,0x12,0x0F,                    //Q
	0x1F,0x14,0x0B,                    //R
	0x09,0x15,0x12,                    //S
	0x10,0x1F,0x10,                    //T
	0x1F,0x01,0x1F,                    //U
	0x1E,0x01,0x1E,                    //V
	0x1F,0x06,0x1F,                    //W
	0x1B,0x04,0x1B,                    //X
	0x18,0x07,0x18,                    //Y
	0x13,0x15,0x19,                    //Z
	0x00,0x1F,0x11,                    //[
	 0,0,0,                    // - �������� ����
	0x11,0x1F,0x00,                    //]
	0x04,0x08,0x04,                    //^
	0x01,0x01,0x01,                    //_
	0x00,0x0C,0x00,                    //'
	0x06,0x09,0x07,                    //a
	0x1F,0x05,0x07,                    //b
	0x06,0x09,0x09,                    //c
	0x07,0x05,0x1F,                    //d
	0x0E,0x15,0x08,                    //e
	0x04,0x1F,0x14,                    //f
	0x1D,0x15,0x1E,                    //g
	0x1F,0x04,0x07,                    //h
	0x00,0x17,0x00,                    //i
	0x01,0x17,0x00,                    //j
	0x1F,0x02,0x05,                    //k
	0x10,0x1F,0x00,                    //l
	0x1F,0x08,0x1F,                    //m=M
	0x0F,0x04,0x07,                    //n
	0x06,0x09,0x06,                    //o
	0x0F,0x0A,0x04,                    //p
	0x04,0x0A,0x0F,                    //q
	0x0F,0x04,0x04,                    //r
	0x09,0x15,0x12,                    //s=S
	0x04,0x0F,0x05,                    //t
	0x06,0x01,0x07,                    //u
	0x06,0x01,0x06,                    //v
	0x0F,0x02,0x0F,                    //w=W
	0x0D,0x02,0x0D,                    //x
	0x0D,0x02,0x0C,                    //y
	0x09,0x0B,0x0D,                    //z
	0x04,0x1F,0x11,                    //{
	0x00,0x1F,0x00,                    //|
	0x11,0x1F,0x04,                    //}
	 0,0,0,                    //~
	0x00,0x00,0x00,0x00,0x00,                    //������
	// 0x7f
	//������� ����������(0x60-0xa0)
	0x01,0x00,0x00,                    //A =A
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //� =B
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //E =E
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //K =K
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //M =M
	0x01,0x00,0x00,                    //H =H
	0x01,0x00,0x00,                    //O =O
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //P =P
	0x01,0x00,0x00,                    //C =C
	0x01,0x00,0x00,                    //T =T
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //X =X
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //a =a
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //e
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //o
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //p
	0x01,0x00,0x00,                    //c
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //x
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00,                    //�
	0x01,0x00,0x00                    //�
	//0xFF
};