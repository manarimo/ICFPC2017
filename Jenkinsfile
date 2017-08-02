pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
                sh './build.sh'
            }
        }
        stage ('Run') {
            steps {
                sh './run.sh'
            }
        }
    }
}