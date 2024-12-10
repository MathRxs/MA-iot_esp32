import paho.mqtt.client as mqtt
import ssl

THINGNAME = "need_to_find_a_name"
# Paramètres AWS IoT
AWS_ENDPOINT = "a33fcaseh27un2-ats.iot.eu-west-1.amazonaws.com"  # Remplacez par votre endpoint
PORT = 8883
TOPIC = "$aws/certificates/create/json"  # Remplacez par votre sujet MQTT
CERTIFICATE_PATH = "/home/mathias/master/iot/projet/aws/certs/bd0201bbf873b7fb8274086910692330644e0ad4171cb2579127c0bcda040982-certificate.pem.crt"
PRIVATE_KEY_PATH = "/home/mathias/master/iot/projet/aws/certs/bd0201bbf873b7fb8274086910692330644e0ad4171cb2579127c0bcda040982-private.pem.key"
CA_PATH = "/home/mathias/master/iot/projet/aws/certs/AmazonRootCA1.pem"

# Message à publier
MESSAGE = ""

# Callback lors de la connexion au broker
def on_connect(
    client, userdata, flags, rc):
    if rc == 0:
        print("Connecté avec succès à AWS IoT")
        client.subscribe("$aws/certificates/create/json/accepted")
        # Publier un message
        client.publish(TOPIC, str(MESSAGE))
        print(f"Message publié : {MESSAGE}")
    else:
        print(f"Échec de connexion, code de retour : {rc}")

# Callback lors de la publication d'un message
def on_publish(client, userdata, mid):
    print(f"Message publié avec l'ID : {mid}")
def on_message(client, userdata, message):
    print("Message reçu : ", str(message.payload.decode("utf-8")))
    print("Sujet du message : ", message.topic)
    print("QoS du message : ", message.qos)
    print("Retained : ", message.retain)

# Configuration du client MQTT
client = mqtt.Client(client_id=THINGNAME)
client.on_connect = on_connect
client.on_publish = on_publish
client.on_message = on_message

# Configurer les certificats de sécurité
client.tls_set(
    CA_PATH,
    certfile=CERTIFICATE_PATH,
    keyfile=PRIVATE_KEY_PATH,
    tls_version=ssl.PROTOCOL_TLSv1_2,
)

# Se connecter au broker AWS IoT
client.connect(AWS_ENDPOINT, PORT)

# Boucle pour gérer les événements MQTT
client.loop_forever()
