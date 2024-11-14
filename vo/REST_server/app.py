from flask import Flask, render_template, request, jsonify, abort
import serial
import json

# Initialisation des données pour les capteurs
tab_T = []  # Tableau pour stocker les valeurs de température
tab_P = []  # Tableau pour stocker les valeurs de pression
welcome = "Welcome to 3ESE API!"  # Message d'accueil initial

# Création de l'application Flask
app = Flask(__name__)

# ========================== Configuration du port série ==========================
# Essayer de se connecter au port série pour communiquer avec le STM32.
try:
    # Configuration de la connexion série avec un débit de 115200 bauds
    ser = serial.Serial("/dev/ttyAMA0", 115200, timeout=1)
    # Réinitialisation des buffers d'entrée et de sortie
    ser.reset_output_buffer()
    ser.reset_input_buffer()
except serial.SerialException as e:
    # Si la connexion série échoue, afficher une erreur et définir ser à None
    print(f"Erreur de connexion au port série : {e}")
    ser = None  # Indique que le port série n'est pas disponible

# ========================== Routes ==========================

# Route principale pour afficher un message "Hello, World!" en test
@app.route('/')
def hello_world():
    return 'Hello, World!\n'

# ========================== Route pour le message d'accueil ==========================
# Cette route gère le message d'accueil et prend en charge les méthodes GET, POST et DELETE.
@app.route('/api/welcome/', methods=['GET', 'POST', 'DELETE'])
def api_welcome():
    global welcome  # Utilisation de la variable globale welcome
    if request.method == 'POST':
        # Si la requête est en POST, on s'attend à recevoir un message dans le corps JSON
        data = request.get_json()
        if 'message' in data:
            welcome = data['message']  # Mettre à jour le message d'accueil
            return jsonify(welcome)    # Retourner le nouveau message d'accueil
    elif request.method == 'GET':
        # Si la requête est en GET, on renvoie le message d'accueil actuel
        return jsonify(welcome)
    elif request.method == 'DELETE':
        # Si la requête est en DELETE, on réinitialise le message d'accueil à une chaîne vide
        welcome = ""
        return jsonify(welcome)

# ========================== Route pour message d'accueil avec index ==========================
# Cette route permet d'interagir avec une lettre spécifique du message d'accueil, selon son index.
@app.route('/api/welcome/<int:index>', methods=['GET', 'POST', 'PUT', 'PATCH', 'DELETE'])
def api_welcome_index(index=None):
    global welcome
    if request.method == 'GET':
        # Récupérer la lettre à l'index spécifié si l'index est valide
        if 0 <= index < len(welcome):
            return jsonify({"index": index, "val": welcome[index]})
        else:
            abort(404)  # Si l'index est hors des limites, renvoyer une erreur 404
    elif request.method == 'POST':
        # Mettre à jour le message d'accueil avec une nouvelle valeur reçue en JSON
        data = request.get_json()
        welcome = data.get('message', welcome)  # Utiliser la nouvelle valeur ou conserver la précédente
        return jsonify(welcome)
    elif request.method == 'PUT':
        # Insérer un mot à un index spécifié dans le message d'accueil
        data = request.get_json()
        word_insert = data.get('word', "")
        if 0 <= index <= len(welcome):
            # Insérer le mot à la position indiquée
            welcome = welcome[:index] + word_insert + welcome[index:]
            return jsonify(welcome)
    elif request.method == 'PATCH':
        # Remplacer une lettre spécifique à un index dans le message d'accueil
        data = request.get_json()
        letter_insert = data.get('letter', "")
        if 0 <= index < len(welcome):
            # Remplacer la lettre à la position spécifiée
            welcome = welcome[:index] + letter_insert + welcome[index+1:]
            return jsonify(welcome)
    elif request.method == 'DELETE':
        # Supprimer une lettre à un index spécifié dans le message d'accueil
        if 0 <= index < len(welcome):
            welcome = welcome[:index] + welcome[index+1:]
            return jsonify(welcome)

# ========================== Gestion des erreurs 404 ==========================
# Route pour gérer les erreurs 404 en affichant une page personnalisée
@app.errorhandler(404)
def page_not_found(error):
    # Renvoie une page d'erreur personnalisée avec un statut 404
    return render_template('page_not_found.html'), 404

# ========================== Route pour les requêtes générales ==========================
# Route permettant de tester et de renvoyer les informations de la requête
@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET', 'POST'])
def api_request(path=None):
    # Structure de réponse contenant des informations sur la requête
    resp = {
        "method": request.method,
        "url": request.url,
        "path": path,
        "args": request.args,
        "headers": dict(request.headers),
    }
    if request.method == 'POST':
        # Ajouter les données reçues en POST à la réponse
        resp["POST"] = {"data": request.get_json()}
    return jsonify(resp)

# ========================== Gestion des températures ==========================
# Route pour gérer les opérations de température
@app.route('/api/temp/', methods=['GET', 'POST'])
def api_temp():
    # Vérification de la disponibilité du port série
    if not ser:
        return jsonify({"error": "Série non disponible"}), 500
    # Réinitialisation des buffers
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    if request.method == 'POST':
        # Commande GET_T envoyée au STM32 pour récupérer la température
        ser.write(b'GET_T')
        # Lire la valeur envoyée par le STM32 et ajouter au tableau
        tempo = ser.readline().decode().strip()  # Retire le \r\n
        tab_T.append(tempo)
        return jsonify(tab_T[-1])  # Renvoie la dernière valeur ajoutée
    elif request.method == 'GET':
        return jsonify(tab_T)  # Renvoie tout le tableau de températures

@app.route('/api/temp/<int:index>', methods=['GET', 'DELETE'])
def api_temp_index(index=None):
    if request.method == 'GET':
        # Récupérer la température à l'index spécifié
        if 0 <= index < len(tab_T):
            return jsonify(tab_T[index])
        else:
            return jsonify("error: index out of range"), 404
    elif request.method == 'DELETE':
        # Supprimer une température à l'index spécifié
        if 0 <= index < len(tab_T):
            removed = tab_T.pop(index)
            return jsonify(f"la valeur {removed} a été retirée")
        else:
            return jsonify("error: index out of range"), 404

# ========================== Gestion des pressions ==========================
# Route pour gérer les opérations de pression
@app.route('/api/pres/', methods=['GET', 'POST'])
def api_pres():
    if not ser:
        return jsonify({"error": "Série non disponible"}), 500
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    if request.method == 'POST':
        # Commande GET_P envoyée pour obtenir la pression
        ser.write(b'GET_P')
        tempo = ser.readline().decode().strip()
        tab_P.append(tempo)
        return jsonify(tab_P[-1])  # Renvoie la dernière valeur de pression
    elif request.method == 'GET':
        return jsonify(tab_P)  # Renvoie tout le tableau de pressions

@app.route('/api/pres/<int:index>', methods=['GET', 'DELETE'])
def api_pres_index(index=None):
    if request.method == 'GET':
        if 0 <= index < len(tab_P):
            return jsonify(tab_P[index])
        else:
            return jsonify("error: index out of range"), 404
    elif request.method == 'DELETE':
        if 0 <= index < len(tab_P):
            removed = tab_P.pop(index)
            return jsonify(f"la valeur {removed} a été retirée")
        else:
            return jsonify("error: index out of range"), 404

# ========================== Gestion de l'échelle ==========================
# Route pour obtenir le coefficient d'échelle depuis le STM32
@app.route('/api/scale/', methods=['GET'])
def api_scale():
    if not ser:
        return jsonify({"error": "Série non disponible"}), 500
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    ser.write(b'GET_K')  # Envoie la commande GET_K pour obtenir le coefficient
    tempo = ser.readline().decode().strip()  # Lit et nettoie la réponse
    return jsonify(tempo)

@app.route('/api/scale/<int:index>', methods=['POST'])
def api_scale_index(index=None):
    if not ser:
        return jsonify({"error": "Série non disponible"}), 500
    ser.write(b'SET_K')  # Envoie la commande SET_K pour définir le coefficient
    ser.write(str(index).encode())  # Envoie la valeur de l'index
    return jsonify(f"le nouveau coeff est {index}")

# ========================== Gestion de l'angle ==========================
# Route pour afficher une indication de l'avancement de la fonctionnalité de gestion d'angle
@app.route('/api/angle/', methods=['GET'])
def api_angle():
    return jsonify("work in progress")  # Fonctionnalité non implémentée



@app.route('/dashboard')
def dashboard():
    return render_template('dashboard.html')

# ========================== Lancement du serveur Flask ==========================
# Lance le serveur sur toutes les interfaces réseau, avec le port 5000, en mode débogage
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
