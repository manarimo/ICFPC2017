pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
                sh './bin/build.sh'
            }
        }
        stage ('Run') {
            steps {
                sh './bin/run.sh'
            }
        }
    }
}