import boto3
import json
import os
import serial
import time
import sys


aws = boto3.client('iot',None)
# Créer un device
def create_device(device_name):
    response = aws.create_thing(thingName=device_name)
    return response
# Créer un certificat   
def creat_and_save_cretificat(thing_name):
    """
    It creates a certificate, saves the public and private keys, and returns the certificate ARN.
    
    :param thing_name: The name of the thing witch be attach to this certficate
    :return: The certificateArn is being returned.
    """
    # certResponse = aws.create_keys_and_certificate(setAsActive = True)
    os.mkdir("./Certificates/"+thing_name)
    data = json.loads(json.dumps(certResponse, sort_keys=False, indent=4))
    certificateArn = data['certificateArn']
    PublicKey = data['keyPair']['PublicKey']
    PrivateKey = data['keyPair']['PrivateKey']
    certificatePem = data['certificatePem']
    with open( "./Certificates/" + thing_name +"/public.key", 'w') as outfile:
            outfile.write(PublicKey)
    with open("./Certificates/" + thing_name +"/private.key", 'w') as outfile:
            outfile.write(PrivateKey)
    with open("./Certificates/" + thing_name + "/cert.pem", 'w') as outfile:
            outfile.write(certificatePem)
    return certificateArn

def attach_policy_and_certificat_to_the_thing(thing_name, certificateArn):           
    """
    This function attaches the policy and certificate to the thing
    
    :param thing_name: The name of the thing you want to attach the certificate to
    :param certificateArn: The ARN of the certificate to attach to the thing
    """
    response = aws.attach_policy(policyName = "aerus_claim_cerficates",target = certificateArn)
    response = aws.attach_thing_principal(thingName = thing_name,principal = certificateArn)



def provision_cert(serial, filename_private_cert, filename_public_cert):
    public_cert = open(filename_public_cert, "r")
    public_cert_txt = "\""+public_cert.read()
    if(public_cert_txt[-1]=='\n'):
        public_cert_txt= public_cert_txt[:-1]
    public_cert_txt = bytes(public_cert_txt, 'utf-8')

    print(filename_private_cert)
    private_cert = open(filename_private_cert, "r")
    private_cert_txt = "\""+private_cert.read()
    if(private_cert_txt[-1]=='\n'):
        private_cert_txt = private_cert_txt[:-1]
    private_cert_txt = bytes(private_cert_txt, 'utf-8')

    serial.write(public_cert_txt)
    time.sleep(0.5)
    serial.write(private_cert_txt)

# main
if len(sys.argv) != 2:
    print("Usage: python cert_prov.py <thing_name>")
    sys.exit(1)
thingname = sys.argv[1]
# create_device(thingname)
serial_dev = serial.Serial('/dev/ttyACM0', 115200)
# certArn = creat_and_save_cretificat(thingname)
# attach_policy_and_certificat_to_the_thing(thingname, certArn)
provision_cert(serial_dev, "./Certificates/"+thingname+"/private.key", "./Certificates/"+thingname+"/public.key")
serial_dev.close()

    
