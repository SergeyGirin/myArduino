

class Splitter
{
  public:
  String str;
  String splitter;
  int parts;
  int begin(String source, String split);
  String part(int p);
};

int Splitter::begin(String source, String split) {
  splitter = split;
  str = source;
  String y = source;
  //Serial.println(y); 
  
  int count = -1;
  while (y.indexOf(splitter)>0)
  {
    count++;
    int p = y.indexOf(splitter);   
    String x = y.substring(0,p);
    y = y.substring(p+1);
  }
  parts = count;
  return count;
}

String Splitter::part(int part) {
  String y = str;
  //Serial.println(y); 
  
  int count = -1;
  while (y.indexOf(splitter)>0)
  {
    count++;
    int p = y.indexOf(splitter);   
    String result = y.substring(0,p);
    y = y.substring(p+1);
    
    if (count == part)
    {
      count = -1;
      return result;
    }
    
  }
}
  
 








void test()
{
  Splitter line;
  Serial.println(line.begin("012;345;67;89;",";"));
  for (int i=0;i<=line.parts;i++)
  {
    Serial.println(line.part(i));
  }
  Serial.println("-------------");
  
  Splitter www;
  Serial.println(www.begin("012#345#67#89#222#deee#","#"));
  for (int i=0;i<=www.parts;i++)
  {
    Serial.println(www.part(i));
  }
 

}

void setup() {
  Serial.begin(115200);
  Serial.println();  
  Serial.println();
  Serial.println();
  Serial.println("******************");
  test();
  Serial.println("******************");
  
}

void loop() {

}
