from operator import itemgetter
"""
We need to print this struct for each entry
  // #no.entry
  {
    .in_register=&PIN#param1
    .out_register=&PORT#param1
    .dir_register=&DDR#param1
    .bit=#param2
    .tcc_register=0,
    .oc_register=0,
    .com_mask=0
  },
"""

def temp_print(data):
  noentry=data[0]
  param1=data[1]
  param2=data[2]
  temp="\t\t//{0}\n" \
       "\t\t{{\n" \
       "\t\t\t.in_register=&GPIO{1}_PDIR,\n" \
       "\t\t\t.out_register=&GPIO{1}_PDOR,\n" \
       "\t\t\t.dir_register=&GPIO{1}_PDDR,\n" \
       "\t\t\t.bit={2},\n" \
       "\t\t\t.tcc_register=0,\n" \
       "\t\t\t.oc_register=0,\n" \
       "\t\t\t.com_mask=0,\n" \
       "\t\t}},\n".format(noentry, param1, param2)
  return temp


pin_list = list()

print("Usage :> #no.entry #register #bit")
print("         type 'exit' to close the operation")



while(True):
  usr_input=input("")
  if usr_input is 'exit' or len(usr_input) is 0:
    break
  lines = usr_input.split('\n')
  for i in lines:
    parsed_input=i.split(" ")
    print('parsed [{}, {}, {}]'.format(parsed_input[0], parsed_input[1], parsed_input[2]))
    parsed_input[0] = int(parsed_input[0])
    pin_list.append(parsed_input)

f=open('template.txt', mode='w')

f.write('const Pin pins[] =\n\t{')
print("operation terminated\ngenerating template")
pin_list = sorted(pin_list, key=itemgetter(0))
for pin in pin_list:
  f.write(temp_print(pin))
f.write('\t};\n')


