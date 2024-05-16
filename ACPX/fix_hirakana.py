import os
halfgana='!?｡｢｣､･ｦｧｨｩｪｫｬｭｮｯｰｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝﾞﾟ'
fullgana='！？　。「」、…をぁぃぅぇぉゃゅょっーあいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわん゛゜'
Ganatbl={}
for i in range(len(halfgana)):
  Ganatbl[halfgana[i]]=fullgana[i]

fs=open("test.txt","rb");
stm=fs.read().decode('932').split("\r\n")
ret=[]
for l in stm:
  ns=[]
  l=l.strip()
  for c in l:
    if c in Ganatbl:
      ns.append(Ganatbl[c])
    else:
      ns.append(c)
  ret.append(''.join(ns))

fs=open("txt.txt","wb")
fs.write("\r\n".join(ret).encode("u16"))
fs.close()
      
